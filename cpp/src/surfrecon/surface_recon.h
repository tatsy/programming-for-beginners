#pragma once

#include <vector>
#include "common/vec3.h"

void surfaceFromPoints(const std::vector<Vec3> &points, const std::vector<Vec3> &normals,
                       std::vector<Vec3> *outVerts, std::vector<uint32_t> *outFaces);