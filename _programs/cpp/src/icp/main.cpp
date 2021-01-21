#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "common/vec3.h"

// Load OFF mesh file (in this program, the file stores only point cloud)
void read_off(const std::string &filename, std::vector<Vec3> *positions, std::vector<Vec3> *normals = nullptr) {
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
        if (normals) {
            normals->push_back(n);
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        fprintf(stderr, "[ USAGE ] icp_exe [ *.off file ] [ *.off file ]\n");
        std::exit(1);
    }

    // Load point cloud data
    std::vector<Vec3> pos0;
    std::vector<Vec3> pos1;
    read_off(argv[1], &pos0);
    read_off(argv[2], &pos1);

    printf("PCL #0: %ld points\n", pos0.size());
    printf("PCL #1: %ld points\n", pos1.size());
}