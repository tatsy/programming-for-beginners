#include <iostream>
#include <vector>

#include "volume.h"
#include "mcubes.h"
#include "trimesh.h"

int main(int argc, char **argv) {
    if (argc <= 4) {
        fprintf(stderr, "[ USAGE ] march_cubes [ *.vol file ] [ width ] [ height ] [ dims ]");
        std::exit(1);
    }

    // Load volume data
    const int sizeX = atoi(argv[2]);
    const int sizeY = atoi(argv[3]);
    const int sizeZ = atoi(argv[4]);
    Volume vol(argv[1], sizeX, sizeY, sizeZ);
    printf("Size: %lld x %lld x %lld\n", vol.size(0), vol.size(1), vol.size(2));

    // Marching cubes
    std::vector<Vec3> positions;
    std::vector<uint32_t> indices;
    marchCubes(vol, &positions, &indices, -1.0, true);

    // Write mesh data
    write_obj("output.obj", positions, indices);
    write_ply("output.ply", positions, indices);
}
