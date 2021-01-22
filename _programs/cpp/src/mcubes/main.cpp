#include <iostream>
#include <vector>

#include "common/path.h"
#include "common/volume.h"
#include "common/io.h"
#include "mcubes.h"

int main(int argc, char **argv) {
    if (argc <= 4) {
        fprintf(stderr, "[ USAGE ] march_cubes [ *.vol file ] [ width ] [ height ] [ dims ]");
        std::exit(1);
    }

    // Output path
    filepath path(argv[1]);
    const filepath dirname = path.dirname();
    const filepath basename = path.stem();
    const std::string outfile = (dirname / basename + ".ply").string();

    // Load volume data
    const int sizeX = std::atoi(argv[2]);
    const int sizeY = std::atoi(argv[3]);
    const int sizeZ = std::atoi(argv[4]);
    Volume vol(argv[1], sizeX, sizeY, sizeZ);
    printf("Size: %lld x %lld x %lld\n", vol.size(0), vol.size(1), vol.size(2));

    // Marching cubes
    std::vector<Vec3> positions;
    std::vector<uint32_t> indices;
    marchCubes(vol, &positions, &indices, -1.0, true);
    //marchTets(vol, &positions, &indices, -1.0, true);
    //dualContour(vol, &positions, &indices, -1.0, true);

    // Write mesh data
    write_ply(outfile, positions, indices);
    printf("Saved to: %s\n", outfile.c_str());
}
