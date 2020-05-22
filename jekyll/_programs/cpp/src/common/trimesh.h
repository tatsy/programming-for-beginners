#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "vec3.h"

void write_obj(const std::string &filename, const std::vector<Vec3> &positions, const std::vector<uint32_t> &indices) {
    std::ofstream writer(filename.c_str(), std::ios::out);
    if (writer.fail()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    for (const auto &p : positions) {
        writer << "v " << p.x << " " << p.y << " " << p.z << "\n";
    }

    const int nFaces = (int)indices.size() / 3;
    for (int i = 0; i < nFaces; i++) {
        const uint32_t a = indices[i * 3 + 0] + 1;
        const uint32_t b = indices[i * 3 + 1] + 1;
        const uint32_t c = indices[i * 3 + 2] + 1;
        writer << "f " << a << " " << b << " " << c << "\n";
    }

    writer.close();
}

void write_ply(const std::string &filename, const std::vector<Vec3> &positions, const std::vector<uint32_t> &indices) {
    std::ofstream writer(filename.c_str(), std::ios::out | std::ios::binary);
    if (writer.fail()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    writer << "ply" << "\n";
    writer << "format binary_little_endian 1.0" << "\n";

    const size_t nVerts = positions.size();
    writer << "element vertex " << nVerts << "\n";
    writer << "property float x" << "\n";
    writer << "property float y" << "\n";
    writer << "property float z" << "\n";

    const int nFaces = (int)indices.size() / 3;
    writer << "element face " << nFaces << "\n";
    writer << "property list uchar int vertex_indices" << "\n";
    writer << "end_header" << "\n";

    for (const auto &p : positions) {
        float buf[3] = {(float)p.x, (float)p.y, (float)p.z};
        writer.write((char*)buf, sizeof(float) * 3);
    }

    for (int i = 0; i < nFaces; i++) {
        const uint8_t k = 3;
        writer.write((char*)&k, sizeof(uint8_t));

        const uint32_t a = indices[i * 3 + 0];
        const uint32_t b = indices[i * 3 + 1];
        const uint32_t c = indices[i * 3 + 2];
        uint32_t buf[3] = {a, b, c};
        writer.write((char*)buf, sizeof(uint32_t) * 3);
    }

    writer.close();
}
