#include <cstdlib>
#include <vector>

#include "common/vec3.h"
#include "common/io.h"
#include "common/path.h"
#include "icp.h"

int main(int argc, char **argv) {
    if (argc <= 2) {
        fprintf(stderr, "[ USAGE ] icp_exe [ *.off file ] [ *.off file ] [ iteration ] [ tolerance ]\n");
        std::exit(1);
    }

    const int nIterations = argc > 3 ? atoi(argv[3]) : 100;
    const double tolerance = argc > 4 ? atof(argv[4]) : 1.0e-4;

    try {
        // Load point cloud data
        std::vector<Vec3> pos0;
        std::vector<Vec3> norm0;
        std::vector<Vec3> pos1;
        std::vector<Vec3> norm1;
        read_off(argv[1], &pos0, &norm0);
        read_off(argv[2], &pos1, &norm1);
        printf("PCL #0: %ld points\n", pos0.size());
        printf("PCL #1: %ld points\n", pos1.size());

        // Rigid ICP
        rigidICP(pos0, norm0, pos1, norm1, ICPMetric::Point2Plane, nIterations, tolerance, true);

        // Write as *.off
        filepath path(argv[2]);
        const filepath dirname = path.dirname();
        const filepath basename = path.stem();
        const std::string outfile = (dirname / basename + "_output.off").string();
        write_off(outfile, pos1, norm1);
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    }
}