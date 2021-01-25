#include "icp.h"

#include <iostream>

#include <Eigen/Dense>

using FloatType = double;
using IndexType = int64_t;
using EigenMatrix = Eigen::MatrixXd;
using EigenVector = Eigen::VectorXd;

#include "common/io.h"
#include "common/kdtree.h"
#include "common/debug.h"
#include "svd.h"

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

    const double wx = w(0);
    const double wy = w(1);
    const double wz = w(2);
    EigenMatrix K(3, 3);
    K << 0.0, -wz, wy, wz, 0.0, -wx, -wy, wx, 0.0;

    EigenMatrix I = EigenMatrix::Identity(3, 3);
    return I + K * s + (K * K) * (1.0 - c);
}

//! single point to point ICP step
void point2pointICP_step(const std::vector<Vec3> &target, const std::vector<Vec3> &source, EigenMatrix *rotMat,
                         EigenVector *trans) {
    // Construct Kd-tree to find closest points
    KDTree<Vec3> tree;
    tree.construct(target);

    // {{ NOT_IMPL_ERROR();

    // Match closest point pairs
    // Hint:
    // Construct here that two matrices X and P,
    // whose elements are positions of source vertices (X),
    // and those for closest points of the sources (P)
    const int nPoints = (int)source.size();
    EigenMatrix X(nPoints, 3);
    EigenMatrix P(nPoints, 3);
    for (int i = 0; i < nPoints; i++) {
        const Vec3 &v = source[i];
        const Vec3 u = tree.nearest(v);
        X.row(i) << v.x, v.y, v.z;
        P.row(i) << u.x, u.y, u.z;
    }

    // Subtract xMean, pMean from X, P
    // Hint:
    // Row-wise or column-wise mean (also sum, max, min etc.) of Eigen matrices
    // can be easily calculate using "rowwise()" or "colwise()".
    const EigenVector xMean = X.colwise().mean();
    const EigenVector pMean = P.colwise().mean();
    X.rowwise() -= xMean.transpose();
    P.rowwise() -= pMean.transpose();

    // Solve with SVD to obtain R
    // Hint:
    // An SVD method "eigenSVD" is already given in "svd.h".
    // See its arguments carefully when using it.
    const EigenMatrix M = P.transpose() * X;
    EigenMatrix U, V;
    EigenVector sigma;
    eigenSVD(M, U, sigma, V);
    const double detVU = (U * V.transpose()).determinant();

    // Revise sign of determinant
    EigenVector diagH(3);
    diagH << 1.0, 1.0, detVU;

    // Output
    // Hint:
    // Be careful that arguments for output matrices and vectors
    // are represented as "pointers". Therefore, you should insert
    // values for them with "asterisk" like "*lhs = rhs".
    *rotMat = U * diagH.asDiagonal() * V.transpose();
    *trans = pMean - (*rotMat) * xMean;

    // }}
}

//! single point to plane ICP step
void point2planeICP_step(const std::vector<Vec3> &target, const std::vector<Vec3> &targetNorm,
                         const std::vector<Vec3> &source, Eigen::MatrixXd *rotMat, Eigen::VectorXd *trans) {
    // Construct Kd-tree to find closest points
    std::vector<Point> points;
    for (int i = 0; i < (int)target.size(); i++) {
        points.emplace_back(target[i], i);
    }

    KDTree<Point> tree;
    tree.construct(points);

    // {{ NOT_IMPL_ERROR();

    // Construct linear system
    // Hint:
    // prepare matrix A and vector b
    // A is 6x6 matrix, and b is 6-D vector
    const int nPoints = (int)source.size();
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

    // Solve
    // Hint:
    // Use "Eigen::PartialPivLU" to solve the system
    Eigen::PartialPivLU<EigenMatrix> lu(A);
    EigenVector u = lu.solve(b);

    // Substitute to instances a, t
    // Hint:
    // Eigen matrices and vectors can be initialized
    // with its elements by "<<" operator.
    EigenVector a(3);
    a << u(0), u(1), u(2);
    EigenVector t(3);
    t << u(3), u(4), u(5);

    // Reproduce rotation matrix
    // Hint:
    // Remember that vector "a" is the multiple of
    // the direction of rotation axis and
    // the degree of rotation angle.
    const double theta = a.norm();
    const EigenVector w = a / theta;
    *rotMat = rodrigues(w, theta);
    *trans = t;

    // }}
}

void rigidICP(const std::vector<Vec3> &target, const std::vector<Vec3> &targetNorm, std::vector<Vec3> &source,
              std::vector<Vec3> &sourceNorm, ICPMetric metric, int maxIters, double tolerance, bool verbose) {
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

        char outfile[256];
        sprintf(outfile, "intermediate_%03d.off", it);
        write_off(std::string(outfile), source, sourceNorm);

        if (error < tolerance) {
            break;
        }
    }
}
