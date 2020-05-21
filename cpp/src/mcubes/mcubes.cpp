#include "mcubes.h"

#include <climits>
#include <algorithm>
#include <unordered_map>

#include "common/progress.h"
#include "mcubes_utils.h"

double getThresholdOtsu(const Volume &volume) {
    //NOT_IMPL_ERROR();
    const double total = volume.size(0) * volume.size(1) * volume.size(2);
    double hist[USHRT_MAX];
    std::memset(hist, 0, sizeof(hist));
    for (uint64_t z = 0; z < volume.size(2); z++) {
        for (uint64_t y = 0; y < volume.size(1); y++) {
            for (uint64_t x = 0; x < volume.size(0); x++) {
                const uint16_t val = volume(x, y, z);
                if (val != 0) {
                    hist[val] += 1;
                }
            }
        }
    }

    double sum1 = 0.0;
    double c1 = 0.0;
    for (int i = 0; i < USHRT_MAX; i++) {
        hist[i] /= total;
        sum1 += hist[i] * i / (double)USHRT_MAX;
        c1 += hist[i];
    }

    double sum2 = 0.0;
    double c2 = 0.0;
    double maxVar = 0.0;
    double threshold = 0.0;
    for (int i = 0; i < USHRT_MAX; i++) {
        const double mu1 = c1 != 0 ? sum1 / c1 : 0.0;
        const double mu2 = c2 != 0 ? sum2 / c2 : 0.0;

        const double diff = mu1 - mu2;
        const double var = c1 * c2 * diff * diff;
        if (maxVar < var) {
            maxVar = var;
            threshold = i / (double)USHRT_MAX;
        }

        sum1 -= hist[i] * i / (double)USHRT_MAX;
        c1 -= hist[i];
        sum2 += hist[i] * i / (double)USHRT_MAX;
        c2 += hist[i];
    }

    return threshold;
}

void marchCubes(const Volume &volume, std::vector<Vec3> *vertices, std::vector<uint32_t> *indices, double threshold, bool flipFaces) {
    // Clear arrays
    vertices->clear();
    indices->clear();

    // Compute threshold with Otsu's method, if threshold is not specified.
    if (threshold < 0.0) {
        threshold = getThresholdOtsu(volume);
    }
    printf("Threshold: %.5f\n", threshold);

    // Marching cubes
    static int indexTable[8] = { 0, 1, 4, 5, 3, 2, 7, 6 };
    GRIDCELL cell;
    TRIANGLE tris[16];
    const Vec3 resolution = Vec3(1.0, 1.0, 1.0);
    ProgressBar pbar((volume.size(1) - 1) * (volume.size(2) - 1));

    std::unordered_map<Vec3, uint32_t> uniqueVertices;
    for (uint64_t z = 0; z < volume.size(2) - 1; z++) {
        for (uint64_t y = 0; y < volume.size(1) - 1; y++) {
            for (uint64_t x = 0; x < volume.size(0) - 1; x++) {
                //NOT_IMPL_ERROR();
                for (int i = 0; i < 8; i++) {
                    const int dx = (i >> 0) & 0x01;
                    const int dy = (i >> 1) & 0x01;
                    const int dz = (i >> 2) & 0x01;
                    cell.p[indexTable[i]] = Vec3(x + dx, y + dy, z + dz) * resolution;
                    cell.val[indexTable[i]] = volume(x + dx, y + dy, z + dz) / (double)USHRT_MAX;
                }

                std::memset(tris, 0, sizeof(tris));
                int ntris = Polygonise(cell, threshold, tris);

                for (int i = 0; i < ntris; i++) {
                    uint32_t tri[3];
                    for (int j = 0; j < 3; j++) {
                        const int k = flipFaces ? 2 - j : j;
                        const Vec3 &v = tris[i].p[k];
                        if (uniqueVertices.count(v) == 0) {
                            uniqueVertices[v] = static_cast<uint32_t>(vertices->size());
                            vertices->push_back(v);
                        }
                        tri[j] = uniqueVertices[v];
                    }

                    if (tri[0] != tri[1] && tri[0] != tri[2] && tri[1] != tri[2]) {
                        indices->push_back(tri[0]);
                        indices->push_back(tri[1]);
                        indices->push_back(tri[2]);
                    }
                }
            }
            pbar.step();
        }
    }

    printf("#vert: %d\n", (int)vertices->size());
    printf("#face: %d\n", (int)indices->size() / 3);
}
