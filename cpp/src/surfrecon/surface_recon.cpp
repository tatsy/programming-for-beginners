#include "surface_recon.h"

#include <iostream>
#include <random>
#include <algorithm>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <Eigen/Dense>
#include <Eigen/Sparse>

using FloatType = double;
using IndexType = int64_t;
using EigenMatrix = Eigen::MatrixXd;
using EigenVector = Eigen::VectorXd;
using SparseMatrix = Eigen::SparseMatrix<FloatType, Eigen::ColMajor, IndexType>;
using Triplet = Eigen::Triplet<FloatType, IndexType>;

#include "common/kdtree.h"
#include "common/progress.h"
#include "common/volume.h"
#include "mcubes/mcubes.h"

// Wendland's RBF
// Morse et al. 2001,
// "Interpolating Implicit Surfaces From Scattered Surface Data
//  Using Compactly Supported Radial Basis Functions"
inline double csrbf(const Vec3 &x, const Vec3 &y, double s = 0.1) {
    const double r = length(x - y) / s;
    const double a = std::max(0.0, 1.0 - r);
    const double b = 4.0 * r + 1.0;
    return (a * a * a * a) * b;
}

// Custom struct for KD tree
struct Point : public Vec3 {
    Point() {}
    Point(const Vec3 &v, int index = -1) : Vec3(v), i(index) {}
    int i;
};

void surfaceFromPoints(const std::vector<Vec3> &positions, const std::vector<Vec3> &normals,
                       std::vector<Vec3> *outVerts, std::vector<uint32_t> *outFaces,
                       double suppRadius, int mcubeDivs) {

    // In this program, point cloud is first scaled and translated to be inside [-0.5, 0.5]^3 regular cube.
    // This prevents to adjust parameters for CS-RBF or off-surface positions.

    const int nPoints = (int)positions.size();
    printf("#point: %d\n", nPoints);

    // Compute bounding box
    double minX = 1.0e20, maxX = -1.0e20;
    double minY = 1.0e20, maxY = -1.0e20;
    double minZ = 1.0e20, maxZ = -1.0e20;
    for (int i = 0; i < nPoints; i++) {
        minX = std::min(minX, positions[i].x);
        maxX = std::max(maxX, positions[i].x);
        minY = std::min(minY, positions[i].y);
        maxY = std::max(maxY, positions[i].y);
        minZ = std::min(minZ, positions[i].z);
        maxZ = std::max(maxZ, positions[i].z);
    }

    const double maxExtent = std::max(maxX - minX, std::max(maxY - minY, maxZ - minZ)) * 1.1;
    const Vec3 center = Vec3(minX + maxX, minY + maxY, minZ + maxZ) * 0.5;
    const Vec3 origin = center - Vec3(maxExtent) * 0.5;
    printf("origin: %f, %f, %f\n", origin.x, origin.y, origin.z);
    printf("center: %f, %f, %f\n", center.x, center.y, center.z);
    printf("size: %f\n", maxExtent);

    // Normalize input data and construct KD tree.
    KDTree<Point> tree;
    std::vector<Point> points;

    // Generate off-surface points
    std::vector<Vec3> xyz;
    std::vector<double> fvals;
    {
        points.clear();
        tree.clear();
        for (int i = 0; i < nPoints; i++) {
            const auto &p = positions[i];
            points.emplace_back((p - center) / maxExtent, i);
        }
        tree.construct(points);

        double dist;
        Point query, near;


        const double jitter = suppRadius * 0.5;
        for (int i = 0; i < nPoints; i++) {
            const auto p = Vec3(points[i]);
            const auto n = normals[points[i].i];

            // On-surface
            xyz.push_back(p);
            fvals.push_back(0.0);

            // Off-surface (outside)
            query = p + n * jitter;
            near = tree.nearest(query);
            dist = dot(query - near, normals[near.i]) / jitter;
            xyz.push_back(query);
            fvals.push_back(dist);

            // Off-surface (inside)
            query = p - n * jitter;
            near = tree.nearest(query);
            dist = dot(query - near, normals[near.i]) / jitter;
            xyz.push_back(query);
            fvals.push_back(dist);
        }
    }

    // Construct a sparse linear system
    const int64_t N = xyz.size();
    SparseMatrix AA(N + 4, N + 4);
    SparseMatrix II(N + 4, N + 4);
    Eigen::VectorXd bb(N + 4);
    std::vector<Triplet> triplets;
    {
        points.clear();
        tree.clear();
        for (size_t i = 0; i < xyz.size(); i++) {
            points.emplace_back(xyz[i], i);
        }
        tree.construct(points);

        for (int64_t i = 0; i < N; i++) {
            std::vector<Point> knn;
            tree.insideBall(xyz[i], suppRadius, &knn);

            for (const auto &v : knn) {
                const double phi = csrbf(xyz[i], v, suppRadius);
                triplets.emplace_back(i, v.i, phi);
            }

            bb(i) = fvals[i];
        }
    }

    for (int64_t i = 0; i < N; i++) {
        const auto &v = xyz[i];
        double pos[4] = {v.x, v.y, v.z, 1.0};
        for (int64_t j = 0; j < 4; j++) {
            triplets.emplace_back(i, N + j, pos[j]);
            triplets.emplace_back(N + j, i, pos[j]);
        }
    }
    AA.setFromTriplets(triplets.begin(), triplets.end());

    // Solve sparse linear system
    printf("Solving linear system...\n");
    printf("  non-zeros: %d\n", (int)AA.nonZeros());
    printf("   mat-size: %d x %d\n", (int)AA.rows(), (int)AA.cols());

    Eigen::BiCGSTAB<SparseMatrix> solver;
    solver.setMaxIterations(500);
    solver.setTolerance(1.0e-12);
    solver.compute(AA);
    const Eigen::VectorXd weights = solver.solve(bb);

    printf("Finish!\n");
    printf("#iterations: %zu\n", solver.iterations());
    printf("#error: %f\n", solver.error());

    // Evaluate values of implicit function at lattice points
    const int div = mcubeDivs;
    Volume volume(div, div, div);

    ProgressBar pbar(div);
    for (int i = 0; i < div; i++) {
        #ifdef _OPENMP
        #pragma omp parallel for
        #endif
        for (int j = 0; j < div; j++) {
            for (int k = 0; k < div; k++) {
                const double px = (i - (div * 0.5)) / div;
                const double py = (j - (div * 0.5)) / div;
                const double pz = (k - (div * 0.5)) / div;
                const Point pos(Vec3(px, py, pz));

                std::vector<Point> knn;
                tree.insideBall(pos, suppRadius, &knn);

                double value = 0.0;
                for (const auto &v : knn) {
                    value += weights[v.i] * csrbf(pos, v, suppRadius);
                }

                value += weights(N + 0) * pos.x;
                value += weights(N + 1) * pos.y;
                value += weights(N + 2) * pos.z;
                value += weights(N + 3);

                value = std::max(-1.0, std::min(value, 1.0));
                value = (value + 1.0) * 0.5;
                volume(i, j, k) = (int)(value * USHRT_MAX);
            }
        }
        pbar.step();
    }

    // Marching cubes to get iso-contour
    marchCubes(volume, outVerts, outFaces, 0.5);

    // Scale and translate back to original domain
    for (auto &p : *outVerts) {
        p = (p / div) * maxExtent + origin;
    }
}