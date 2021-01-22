#include "icp.h"

#include <iostream>
#include <Eigen/Dense>

using FloatType = double;
using IndexType = int64_t;
using EigenMatrix = Eigen::MatrixXd;
using EigenVector = Eigen::VectorXd;

#include "common/kdtree.h"

// Custom struct for KD tree
struct Point : public Vec3 {
    Point() {}
    Point(const Vec3 &v, int index = -1) : Vec3(v), i(index) {}
    int i;
};

// Rodrigues rotation matrix
EigenMatrix rodrigues(const EigenVector &w, const double theta) {
    const double c = std::cos(theta);
    const double s = std::sin(theta);

    const double nx = w(0);
    const double ny = w(1);
    const double nz = w(2);
    EigenMatrix n(3, 3);
    n << 0.0, -nz, ny,
            nz, 0.0, -nx,
            -ny, nx, 0.0;

    EigenMatrix I = EigenMatrix::Identity(3, 3);

    return I + n * s + (n * n) * (1.0 - c);
}

//! single point to point ICP step
void point2pointICP_step(const std::vector<Vec3> &target,
                         const std::vector<Vec3> &source,
                         EigenMatrix *rotMat,
                         EigenVector *trans) {
    // Construct Kd-tree to find closest points
    KDTree<Vec3> tree;
    tree.construct(target);

    // Match closest point pairs
    const int nPoints = (int)source.size();
    EigenMatrix X(nPoints, 3);
    EigenMatrix P(nPoints, 3);
    for (int i = 0; i < nPoints; i++) {
        const Vec3 &v = source[i];
        const Vec3 u = tree.nearest(v);
        X.row(i) << v.x, v.y, v.z;
        P.row(i) << u.x, u.y, u.z;
    }

    const EigenVector xMean = X.colwise().mean();
    const EigenVector pMean = P.colwise().mean();
    X.rowwise() -= xMean.transpose();
    P.rowwise() -= pMean.transpose();

    const EigenMatrix M = P.transpose() * X;
    Eigen::JacobiSVD<EigenMatrix> svd(M, Eigen::ComputeFullU | Eigen::ComputeFullV);
    const EigenMatrix U = svd.matrixU();
    const EigenMatrix V = svd.matrixV();
    const double detVU = (U * V.transpose()).determinant();

    EigenVector diagH(3);
    diagH << 1.0, 1.0, detVU;

    *rotMat = U * diagH.asDiagonal() * V.transpose();
    *trans = pMean - (*rotMat) * xMean;
}

//! single point to plane ICP step
void point2planeICP_step(const std::vector<Vec3> &target,
                         const std::vector<Vec3> &targetNorm,
                         const std::vector<Vec3> &source,
                         Eigen::MatrixXd *rotMat,
                         Eigen::VectorXd *trans) {
    // Construct Kd-tree to find closest points
    std::vector<Point> points;
    for (int i = 0; i < (int)target.size(); i++) {
        points.emplace_back(target[i], i);
    }

    KDTree<Point> tree;
    tree.construct(points);

    const int nPoints = (int) source.size();
    EigenMatrix A(6, 6);
    EigenVector b(6);
    A.setZero();
    b.setZero();
    for (int i = 0; i < nPoints; i++) {
        const Vec3 &x = source[i];
        const Point pt = tree.nearest(x);
        const Vec3 &p = Vec3(pt.x, pt.y, pt.z);
        const Vec3 &n = targetNorm[pt.i];
        const Vec3 x_cross_n = cross(x, n);
        EigenVector v(6);
        v << x_cross_n.x, x_cross_n.y, x_cross_n.z, n.x, n.y, n.z;
        A += v * v.transpose();
        b += v * (dot(n, p - x));
    }

    Eigen::FullPivLU<EigenMatrix> llt(A);
    EigenVector u = llt.solve(b);

    EigenVector a(3);
    a << u(0), u(1), u(2);
    EigenVector t(3);
    t << u(3), u(4), u(5);

    const double theta = a.norm();
    const EigenVector w = a / theta;
    *rotMat = rodrigues(w, theta);
    *trans = t;
}

void rigidICP(const std::vector<Vec3> &target,
              const std::vector<Vec3> &targetNorm,
              std::vector<Vec3> &source,
              std::vector<Vec3> &sourceNorm,
              ICPMetric metric,
              int maxIters,
              double tolerance,
              bool verbose) {
    // Point-to-point ICP
    for (int it = 0; it < maxIters; it++) {
        Eigen::MatrixXd R(3, 3);
        Eigen::VectorXd t(3);
        switch (metric) {
            case ICPMetric::Point2Point:
                point2pointICP_step(target, source, &R, &t);
                break;

            case ICPMetric::Point2Plane:
                point2planeICP_step(target, targetNorm, source, &R, &t);
                break;

            default:
                throw std::runtime_error("Unknown ICP metric type!");
        }

        // Apply rigid transformation
        for (int i = 0; i < (int)source.size(); i++) {
            Eigen::VectorXd v(3);
            v << source[i].x, source[i].y, source[i].z;
            Eigen::VectorXd u = R * v + t;
            source[i] = Vec3(u(0), u(1), u(2));

            Eigen::VectorXd n(3);
            n << sourceNorm[i].x, sourceNorm[i].y, sourceNorm[i].z;
            Eigen::VectorXd m = R * n;
            sourceNorm[i] = Vec3(m(0), m(1), m(2));
        }

        // Check current error
        const double error = t.norm() + (R - Eigen::MatrixXd::Identity(3, 3)).norm();

        // Report
        if (verbose) {
            printf("*** %d iteration ***\n", it + 1);
            printf("R = \n");
            std::cout << R << std::endl;
            printf("t = \n");
            std::cout << t << std::endl;
            printf("error = %f\n", error);
            printf("\n");
        }

        if (error < tolerance) {
            break;
        }
    }
}
