#pragma once

#include <Eigen/Core>

void eigenSVD(const Eigen::MatrixXd &A, Eigen::MatrixXd &U, Eigen::VectorXd &Sigma, Eigen::MatrixXd &V);
