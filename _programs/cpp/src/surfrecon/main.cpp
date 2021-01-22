#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "common/path.h"
#include "common/timer.h"
#include "common/vec3.h"
#include "common/io.h"

#include "surface_recon.h"

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "[ USAGE ] surfrecon [ *.off file ] [ support radius ] [ #mcube divs ] \n");
        std::exit(1);
    }

    const double suppRadius = argc > 2 ? atof(argv[2]) : 0.05;
    const int    mcubeDivs  = argc > 3 ? atoi(argv[3]) : 256;

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
