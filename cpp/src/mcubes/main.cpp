#include <iostream>

#include "volume.h"

int main(int argc, char **argv) {
    if (argc <= 4) {
        fprintf(stderr, "[ USAGE ] march_cubes [ *.vol file ] [ width ] [ height ] [ dims ]");
        std::exit(1);
    }

    const int sizeX = atoi(argv[2]);
    const int sizeY = atoi(argv[3]);
    const int sizeZ = atoi(argv[4]);
    Volume vol(argv[1], sizeX, sizeY, sizeZ);
    printf("Size: %d x %d x %d\n", vol.size(0), vol.size(1), vol.size(2));
}
