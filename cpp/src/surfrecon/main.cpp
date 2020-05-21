#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "common/path.h"
#include "common/timer.h"
#include "common/vec3.h"
#include "common/trimesh.h"

#include "surface_recon.h"

// Load OFF mesh file (in this program, the file stores only point cloud)
void read_off(const std::string &filename, std::vector<Vec3> *positions, std::vector<Vec3> *normals) {
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
        normals->push_back(n);
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "[ USAGE ] surfrecon [ *.off file ] [ support radius ] [ #mcube divs ] \n");
        std::exit(1);
    }

    const double suppRadius = argc >= 2 ? atof(argv[2]) : 0.05;
    const int    mcubeDivs  = argc >= 3 ? atoi(argv[3]) : 256;

    // Load point cloud data
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    read_off(argv[1], &positions, &normals);

    // Surface reconstruction
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;

    Timer timer;
    timer.start();
    surfaceFromPoints(positions, normals, &vertices, &indices, suppRadius, mcubeDivs);
    // surfaceFromPoints(positions, normals, &vertices, &indices, 0.02, 512);  // For buddha dense
    printf("Time: %f sec\n", timer.stop());

    // Save output mesh
    filepath path(argv[1]);
    const filepath dirname = path.dirname();
    const filepath basename = path.stem();
    const std::string outfile = (dirname / basename + ".ply").string();
    write_ply(outfile, vertices, indices);
}
