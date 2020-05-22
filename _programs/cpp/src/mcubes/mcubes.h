#pragma once

#include <vector>

#include "common/vec3.h"
#include "common/volume.h"

void marchCubes(const Volume &volume, std::vector<Vec3> *vertices, std::vector<uint32_t> *indices,
                double threshold = -1.0, bool flipFaces = false);
