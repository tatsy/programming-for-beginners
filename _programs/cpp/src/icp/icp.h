#pragma once

#include <vector>

#include <Eigen/Core>

#include "common/vec3.h"

enum class ICPMetric {
    Point2Point = 0x00,
    Point2Plane = 0x01,
};

void rigidICP(const std::vector<Vec3> &target,
              const std::vector<Vec3> &targetNorm,
              std::vector<Vec3> &source,
              std::vector<Vec3> &sourceNorm,
              ICPMetric metric,
              int maxIters = 100,
              double tolerance = 1.0e-4,
              bool verbose = false);
