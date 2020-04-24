#pragma once

#include <string>
#include <vector>

#include "vector3d.h"

void mesh_read(const std::string &filename, std::vector<Vector3D> &vertices, std::vector<uint32_t> &indices);

void mesh_write(const std::string &filename, const std::vector<Vector3D> &vertices, const std::vector<uint32_t> &indices);

