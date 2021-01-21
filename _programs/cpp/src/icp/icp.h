#pragma once

#include <vector>

#include <Eigen/Core>

#include "common/vec3.h"

//! Align source point cloud to target point cloud using point-to-point ICP.
void point2pointICP(const std::vector<Vec3> &target,
                    const std::vector<Vec3> &source,
                    Eigen::MatrixXd *rotMat,
                    Eigen::VectorXd *trans);

//! Align source point cloud to target point cloud using point-to-plane ICP.
void point2planeICP(const std::vector<Vec3> &target,
                    const std::vector<Vec3> &targetNorm,
                    const std::vector<Vec3> &source,
                    Eigen::MatrixXd *rotMat,
                    Eigen::VectorXd *trans);