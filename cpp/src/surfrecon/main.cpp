#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <Eigen/Dense>
#include <Eigen/Sparse>

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

#include "vec3.h"
#include "kdtree.h"
#include "volume.h"
#include "trimesh.h"
#include "mcubes.h"
#include "progress.h"

struct Point : public Vec3 {
    Point() {}
    Point(const Vec3 &v) : Vec3(v) {}

    Point(double x, double y, double z, double nx = 0.0, double ny = 0.0, double nz = 0.0)
        : Vec3(x, y, z)
        , nx(nx), ny(ny), nz(nz) {}

    Vec3 normal() const {
        return Vec3(nx, ny, nz);
    }

    double nx = 0.0, ny = 0.0, nz = 0.0;
};

double sign(double x) {
    return x > 0.0 ? 1.0 : -1.0;
}

// Compactly supported RBF
double rbf(const Vec3 &x, const Vec3 &y, double s = 0.2, double k = 1.0) {
    const Vec3 diff = x - y;
    const double norm2 = dot(diff, diff);
    if (norm2 > k * k) {
        return 0.0;
    }
    return std::exp(-0.5 * norm2 / (s * s));
}

void read_off(const std::string &filename, std::vector<Vec3> *positions, std::vector<Vec3> *normals) {
    std::ifstream reader(filename.c_str(), std::ios::in);
    if (reader.fail()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    std::stringstream ss;

    // Magic
    std::getline(reader, line);
    if (line != "NOFF") {
        throw std::runtime_error("Invalid OFF file!");
    }

    // Sizes
    int nVerts, nFaces, nEdges;
    std::getline(reader, line);
    ss.clear();
    ss << line;
    ss >> nVerts >> nFaces >> nEdges;

    // Positions
    while (std::getline(reader, line)) {
        ss.clear();
        ss << line;
        Vec3 v, n;
        ss >> v.x >> v.y >> v.z >> n.x >> n.y >> n.z;
        positions->push_back(v);
        normals->push_back(n);
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "[ USAGE ] surfrecon [ *.off file ]\n");
        std::exit(1);
    }

    // Load point cloud data
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    read_off(argv[1], &positions, &normals);

    const int nPoints = (int)positions.size();
    printf("#point: %d\n", nPoints);

    std::vector<Point> points;
    for (int i = 0; i < nPoints; i++) {
        auto &p = positions[i];
        auto &n = normals[i];
        points.emplace_back(p.x, p.y, p.z, n.x, n.y, n.z);
    }

    KDTree<Point> tree;
    tree.construct(points);

    double dist;
    Point query, near;
    std::vector<Vec3> vertices;
    std::vector<double> distances;

    std::random_device randev;
    std::mt19937 mt(randev());
    std::uniform_real_distribution<double> distrib;

    const double jitter = 0.2;
    for (int i = 0; i < nPoints; i++) {
        const auto p = Vec3(points[i]);
        const auto n = points[i].normal();

        // On-surface
        vertices.push_back(p);
        distances.push_back(0.0);

        // Off-surface (outside)
        query = p + n * jitter; // * distrib(mt);
        near = tree.nearest(query);
        dist = dot(query - near, near.normal());
        dist = sign(dist);
        vertices.push_back(p);
        distances.push_back(dist);

        // Off-surface (inside)
        query = p - n * jitter; // * distrib(mt);
        near = tree.nearest(query);
        dist = dot(query - near, near.normal());
        dist = sign(dist);
        vertices.push_back(p);
        distances.push_back(dist);
    }

    // Construct a sparse linear system
    const int N = vertices.size();
    SparseMatrix AA(N + 4, N + 4);
    Eigen::VectorXd bb(N + 4);
    std::vector<Triplet> triplets;

    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            const double phi = rbf(vertices[i], vertices[j]);
            if (phi != 0.0) {
                triplets.emplace_back(i, j, phi);
                triplets.emplace_back(j, i, phi);
            }
        }
        bb(i) = distances[i];
    }

    for (int i = 0; i < N; i++) {
        const auto &v = vertices[i];
        double pos[4] = {1.0, v.x, v.y, v.z};
        for (int j = 0; j < 4; j++) {
            triplets.emplace_back(i, N + j, pos[j]);
            triplets.emplace_back(N + j, i, pos[j]);
        }
    }
    AA.setFromTriplets(triplets.begin(), triplets.end());
    printf("Size: %d x %d\n", (int)AA.rows(), (int)AA.cols());

    // Solve sparse linear system
    printf("Solving linear system ");
    Eigen::LeastSquaresConjugateGradient<SparseMatrix> solver;
    solver.compute(AA);
    const Eigen::VectorXd coefs = solver.solve(bb);
    printf("-> OK!\n");

    std::cout << "#iterations:     " << solver.iterations() << std::endl;
    std::cout << "estimated error: " << solver.error()      << std::endl;

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

    // Evaluate values of implicit function at latice points
    const int div = 128;
    Volume volume(div, div, div);

    ProgressBar pbar(div);
    for (int i = 0; i < div; i++) {
        #ifdef _OPENMP
        #pragma omp parallel for
        #endif
        for (int j = 0; j < div; j++) {
            for (int k = 0; k < div; k++) {
                const double px = center.x + maxExtent / div * (i - div / 2);
                const double py = center.y + maxExtent / div * (j - div / 2);
                const double pz = center.z + maxExtent / div * (k - div / 2);
                const Vec3 pos(px, py, pz);

                double value = 0.0;
                for (int l = 0; l < N; l++) {
                    value += coefs(l) * rbf(pos, vertices[l]);
                }
                value += coefs(N + 0);
                value += coefs(N + 1) * pos.x;
                value += coefs(N + 2) * pos.y;
                value += coefs(N + 3) * pos.z;

                value = std::max(-1.0, std::min(value, 1.0));
                value = (value + 1.0) * 0.5;

                volume(i, j, k) = (int)(value * USHRT_MAX);
            }
        }
        pbar.step();
    }

    std::vector<Vec3> meshVerts;
    std::vector<uint32_t> meshFaces;
    marchCubes(volume, &meshVerts, &meshFaces);

    for (auto &p : meshVerts) {
        p = (p / div) * maxExtent + origin;
    }

    write_obj("recon.obj", meshVerts, meshFaces);
    write_ply("recon.ply", meshVerts, meshFaces);
}
