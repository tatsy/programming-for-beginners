#include "mcubes.h"

#include <climits>
#include <algorithm>
#include <unordered_map>

// {{
#include <Eigen/Dense>
// }}

#include "common/array3d.h"
#include "common/progress.h"
#include "mcubes_utils.h"

double getThresholdOtsu(const Volume &volume) {
    // {{ NOT_IMPL_ERROR();
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
    // }}
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
    GRIDCELL cell;
    TRIANGLE tris[16];
    const Vec3 resolution = Vec3(1.0, 1.0, 1.0);

    // See: "http://paulbourke.net/geometry/polygonise/"
    // Vertex ordering in the cube is rather wired, and
    // the array below re-orders the vertices for easy bit masking.
    static int indexTable[8] = { 0, 1, 4, 5, 3, 2, 7, 6 };

    ProgressBar pbar((volume.size(1) - 1) * (volume.size(2) - 1));
    std::unordered_map<Vec3, uint32_t> uniqueVertices;
    for (uint64_t z = 0; z < volume.size(2) - 1; z++) {
        for (uint64_t y = 0; y < volume.size(1) - 1; y++) {
            for (uint64_t x = 0; x < volume.size(0) - 1; x++) {
                // {{ NOT_IMPL_ERROR();
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
                // }}
            }
            pbar.step();
        }
    }

    printf("#vert: %d\n", (int)vertices->size());
    printf("#face: %d\n", (int)indices->size() / 3);
}

// {{

void marchTets(const Volume &volume, std::vector<Vec3> *vertices, std::vector<uint32_t> *indices, double threshold, bool flipFaces) {
    // Clear arrays
    vertices->clear();
    indices->clear();

    // Compute threshold with Otsu's method, if threshold is not specified.
    if (threshold < 0.0) {
        threshold = getThresholdOtsu(volume);
    }
    printf("Threshold: %.5f\n", threshold);

    // Marching tetrahedra
    GRIDCELL cell;
    TETRAHEDRON tet;
    TRIANGLE tris[2];
    const Vec3 resolution = Vec3(1.0, 1.0, 1.0);

    // See: "http://paulbourke.net/geometry/polygonise/"
    // Vertex ordering in the cube is rather wired, and
    // the array below re-orders the vertices for easy bit masking.
    static int indexTable[8] = { 0, 1, 4, 5, 3, 2, 7, 6 };
    static const int tetsTable[6][4] = {
            { 6, 0, 5, 1 }, { 6, 0, 4, 5 },
            { 6, 2, 0, 1 }, { 6, 0, 7, 4 },
            { 6, 2, 3, 0 }, { 6, 0, 3, 7 }
    };

    ProgressBar pbar((volume.size(1) - 1) * (volume.size(2) - 1));
    std::unordered_map<Vec3, uint32_t> uniqueVertices;
    for (uint64_t z = 0; z < volume.size(2) - 1; z++) {
        for (uint64_t y = 0; y < volume.size(1) - 1; y++) {
            for (uint64_t x = 0; x < volume.size(0) - 1; x++) {
                for (int i = 0; i < 8; i++) {
                    const int dx = (i >> 0) & 0x01;
                    const int dy = (i >> 1) & 0x01;
                    const int dz = (i >> 2) & 0x01;
                    cell.p[indexTable[i]] = Vec3(x + dx, y + dy, z + dz) * resolution;
                    cell.val[indexTable[i]] = volume(x + dx, y + dy, z + dz) / (double)USHRT_MAX;
                }

                for (int t = 0; t < 6; t++) {
                    for (int j = 0; j < 4; j++) {
                        tet.p[j] = cell.p[tetsTable[t][j]];
                        tet.val[j] = cell.val[tetsTable[t][j]];
                    }

                    std::memset(tris, 0, sizeof(tris));
                    int ntris = PolygonizeTet(tet, threshold, tris);

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
            }
            pbar.step();
        }
    }

    printf("#vert: %d\n", (int)vertices->size());
    printf("#face: %d\n", (int)indices->size() / 3);
}

void dualContour(const Volume &volume, std::vector<Vec3> *vertices, std::vector<uint32_t> *indices, double threshold, bool flipFaces) {
    // Clear arrays
    vertices->clear();
    indices->clear();

    // Compute threshold with Otsu's method, if threshold is not specified.
    if (threshold < 0.0) {
        threshold = getThresholdOtsu(volume);
    }
    printf("Threshold: %.5f\n", threshold);

    // Compute normals
    Array3D<Vec3> normals(volume.size(0), volume.size(1), volume.size(2));
    for (int64_t z = 0; z < (int64_t)volume.size(2); z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1); y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0); x++) {
                const int64_t x0 = std::max(0ll, x - 1);
                const int64_t x1 = std::min(x + 1, (int64_t)volume.size(0) - 1);
                const int64_t y0 = std::max(0ll, y - 1);
                const int64_t y1 = std::min(y + 1, (int64_t)volume.size(1) - 1);
                const int64_t z0 = std::max(0ll, z - 1);
                const int64_t z1 = std::min(z + 1, (int64_t)volume.size(2) - 1);
                const double dx = ((volume(x1, y, z) / (double)USHRT_MAX) - (volume(x0, y, z) / (double)USHRT_MAX)) / (double)(x1 - x0);
                const double dy = ((volume(x, y1, z) / (double)USHRT_MAX) - (volume(x, y0, z) / (double)USHRT_MAX)) / (double)(y1 - y0);
                const double dz = ((volume(x, y, z1) / (double)USHRT_MAX) - (volume(x, y, z0) / (double)USHRT_MAX)) / (double)(z1 - z0);
                if (dx != 0.0 || dy != 0.0 || dz != 0.0) {
                    normals(x, y, z) = normalize(Vec3(dx, dy, dz)) * (flipFaces ? -1.0 : 1.0);
                } else {
                    normals(x, y, z) = Vec3(0.0, 0.0, 0.0);
                }
            }
        }
    }

    // Check intersection between cube edges and iso-contours
    Array3D<std::tuple<Vec3, Vec3>> edges[3];

    // X axis
    edges[0] = Array3D<std::tuple<Vec3, Vec3>>(volume.size(0) - 1, volume.size(1), volume.size(2));
    for (int64_t z = 0; z < (int64_t)volume.size(2); z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1); y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0) - 1; x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x + 1, y, z) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    const Vec3 p0 = Vec3(x, y, z);
                    const Vec3 p1 = Vec3(x + 1, y, z);
                    const Vec3 p = VertexInterp(threshold, p0, p1, v0, v1);
                    const Vec3 n0 = normals(x, y, z);
                    const Vec3 n1 = normals(x + 1, y, z);
                    const Vec3 n = normalize(std::abs(v1 - threshold) * n0 + std::abs(v0 - threshold) * n1);
                    edges[0](x, y, z) = std::make_tuple(p, n);
                }
            }
        }
    }

    // Y axis
    edges[1] = Array3D<std::tuple<Vec3, Vec3>>(volume.size(0), volume.size(1) - 1, volume.size(2));
    for (int64_t z = 0; z < (int64_t)volume.size(2); z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1) - 1; y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0); x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x, y + 1, z) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    const Vec3 p0 = Vec3(x, y, z);
                    const Vec3 p1 = Vec3(x, y + 1, z);
                    const Vec3 p = VertexInterp(threshold, p0, p1, v0, v1);
                    const Vec3 n0 = normals(x, y, z);
                    const Vec3 n1 = normals(x, y + 1, z);
                    const Vec3 n = normalize(std::abs(v1 - threshold) * n0 + std::abs(v0 - threshold) * n1);
                    edges[1](x, y, z) = std::make_tuple(p, n);
                }
            }
        }
    }

    // Z axis
    edges[2] = Array3D<std::tuple<Vec3, Vec3>>(volume.size(0), volume.size(1), volume.size(2) - 1);
    for (int64_t z = 0; z < (int64_t)volume.size(2) - 1; z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1); y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0); x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x, y, z + 1) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    const Vec3 p0 = Vec3(x, y, z);
                    const Vec3 p1 = Vec3(x, y, z + 1);
                    const Vec3 p = VertexInterp(threshold, p0, p1, v0, v1);
                    const Vec3 n0 = normals(x, y, z);
                    const Vec3 n1 = normals(x, y, z + 1);
                    const Vec3 n = normalize(std::abs(v1 - threshold) * n0 + std::abs(v0 - threshold) * n1);
                    edges[2](x, y, z) = std::make_tuple(p, n);
                }
            }
        }
    }

    // Define vertex positions of iso-surface
    using EigenMatrix3 = Eigen::Matrix<double, 3, 3>;
    using EigenVector3 = Eigen::Matrix<double, 3, 1>;
    static const int offset0[4] = { 0, 1, 0, 1 };
    static const int offset1[4] = { 0, 0, 1, 1 };
    Array3D<Vec3> cubes(volume.size(0) - 1, volume.size(1) - 1, volume.size(2) - 1);
    for (int64_t z = 0; z < (int64_t)volume.size(2) - 1; z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1) - 1; y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0) - 1; x++) {
                EigenMatrix3 AA;
                AA.setZero();
                EigenVector3 bb;
                bb.setZero();
                int count = 0;
                Vec3 avg(0.0);

                // X axis
                for (int k = 0; k < 4; k++) {
                    const int64_t nx = x;
                    const int64_t ny = y + offset0[k];
                    const int64_t nz = z + offset1[k];
                    const Vec3 &p = std::get<0>(edges[0](nx, ny, nz)) - Vec3(x, y, z);
                    const Vec3 &n = std::get<1>(edges[0](nx, ny, nz));
                    if (length(p) != 0.0 && length(n) != 0.0) {
                        EigenVector3 nn;
                        nn << n.x, n.y, n.z;
                        EigenVector3 pp;
                        pp << p.x, p.y, p.z;

                        const EigenMatrix3 M = nn * nn.transpose();
                        AA += M;
                        bb += M * pp;
                        avg += p;
                        count += 1;
                    }
                }

                // Y axis
                for (int k = 0; k < 4; k++) {
                    const int64_t nx = x + offset1[k];
                    const int64_t ny = y;
                    const int64_t nz = z + offset0[k];
                    const Vec3 &p = std::get<0>(edges[1](nx, ny, nz)) - Vec3(x, y, z);
                    const Vec3 &n = std::get<1>(edges[1](nx, ny, nz));
                    if (length(p) != 0.0 && length(n) != 0.0) {
                        EigenVector3 nn;
                        nn << n.x, n.y, n.z;
                        EigenVector3 pp;
                        pp << p.x, p.y, p.z;

                        const EigenMatrix3 M = nn * nn.transpose();
                        AA += M;
                        bb += M * pp;
                        avg += p;
                        count += 1;
                    }
                }

                // Z axis
                for (int k = 0; k < 4; k++) {
                    const int64_t nx = x + offset0[k];
                    const int64_t ny = y + offset1[k];
                    const int64_t nz = z;
                    const Vec3 &p = std::get<0>(edges[2](nx, ny, nz)) - Vec3(x, y, z);
                    const Vec3 &n = std::get<1>(edges[2](nx, ny, nz));
                    if (length(p) != 0.0 && length(n) != 0.0) {
                        EigenVector3 nn;
                        nn << n.x, n.y, n.z;
                        EigenVector3 pp;
                        pp << p.x, p.y, p.z;

                        const EigenMatrix3 M = nn * nn.transpose();
                        AA += M;
                        bb += M * pp;
                        avg += p;
                        count += 1;
                    }
                }

                if (count != 0) {
                    Eigen::JacobiSVD<EigenMatrix3> svd;
                    svd.compute(AA, Eigen::ComputeFullU | Eigen::ComputeFullV);
                    double det = 1.0;
                    auto values = svd.singularValues();
                    for (int dim = 0; dim < values.size(); dim++) {
                        const double v = std::abs(values(dim)) < 0.1 ? 0.0 : values(dim);
                        det *= v;
                        values(dim) = v == 0.0 ? 0.0 : 1.0 / v;
                    }
                    const EigenMatrix3 AAinv = svd.matrixU() * values.asDiagonal() * svd.matrixV().transpose();

                    EigenVector3 xx;
                    if (det != 0.0) {
                        xx = AAinv * bb;
                    } else {
                        avg = avg / count;
                        xx << avg.x, avg.y, avg.z;
                    }

//                    EigenVector3 cc;
//                    cc << avg.x, avg.y, avg.z;

//                    const double beta = 1.0e-2;
//                    const EigenVector3 xx = (AA + beta * II).ldlt().solve(bb + beta * cc);

                    const double cx = x + xx(0); //std::max(0.0, std::min(xx(0), 1.0));
                    const double cy = y + xx(1); //std::max(0.0, std::min(xx(1), 1.0));
                    const double cz = z + xx(2); //std::max(0.0, std::min(xx(2), 1.0));
                    cubes(x, y, z) = Vec3(cx, cy, cz);
                } else {
                    cubes(x, y, z) =  Vec3(x + 0.5, y + 0.5, z + 0.5);
                }
            }
        }
    }

    // Dual contouring
    Vec3 rectangle[4];
    int triindex[2][3] = { {0, 1, 3}, {0, 3, 2} };
    std::unordered_map<Vec3, uint32_t> uniqueVertices;

    const int64_t totalSteps = 3 * (volume.size(1) - 1) * (volume.size(2) - 1);
    ProgressBar pbar(totalSteps);

    // X axis
    for (int64_t z = 1; z < (int64_t)volume.size(2); z++) {
        for (int64_t y = 1; y < (int64_t)volume.size(1); y++) {
            for (int64_t x = 0; x < (int64_t)volume.size(0) - 1; x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x + 1, y, z) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    rectangle[0] = cubes(x, y - 1, z - 1);
                    rectangle[1] = cubes(x, y, z - 1);
                    rectangle[2] = cubes(x, y - 1, z);
                    rectangle[3] = cubes(x, y, z);

                    for (int t = 0; t < 2; t++) {
                        uint32_t tri[3];
                        for (int k = 0; k < 3; k++) {
                            const Vec3 &v = (v0 > v1) ? rectangle[triindex[t][k]] : rectangle[triindex[t][3 - k - 1]];
                            if (uniqueVertices.count(v) == 0) {
                                uniqueVertices[v] = static_cast<uint32_t>(vertices->size());
                                vertices->push_back(v);
                            }
                            tri[k] = uniqueVertices[v];
                        }

                        if (tri[0] != tri[1] && tri[0] != tri[2] && tri[1] != tri[2]) {
                            indices->push_back(tri[0]);
                            indices->push_back(tri[1]);
                            indices->push_back(tri[2]);
                        }
                    }
                }
            }
            pbar.step();
        }
    }

    // Y axis
    for (int64_t z = 1; z < (int64_t)volume.size(2); z++) {
        for (int64_t y = 0; y < (int64_t)volume.size(1) - 1; y++) {
            for (int64_t x = 1; x < (int64_t)volume.size(0); x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x, y + 1, z) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    rectangle[0] = cubes(x - 1, y, z - 1);
                    rectangle[1] = cubes(x - 1, y, z);
                    rectangle[2] = cubes(x, y,  z - 1);
                    rectangle[3] = cubes(x, y, z);

                    for (int t = 0; t < 2; t++) {
                        uint32_t tri[3];
                        for (int k = 0; k < 3; k++) {
                            const Vec3 &v = (v0 > v1) ? rectangle[triindex[t][k]] : rectangle[triindex[t][3 - k - 1]];
                            if (uniqueVertices.count(v) == 0) {
                                uniqueVertices[v] = static_cast<uint32_t>(vertices->size());
                                vertices->push_back(v);
                            }
                            tri[k] = uniqueVertices[v];
                        }

                        if (tri[0] != tri[1] && tri[0] != tri[2] && tri[1] != tri[2]) {
                            indices->push_back(tri[0]);
                            indices->push_back(tri[1]);
                            indices->push_back(tri[2]);
                        }
                    }
                }
            }
            pbar.step();
        }
    }

    // Z axis
    for (int64_t z = 0; z < (int64_t)volume.size(2) - 1; z++) {
        for (int64_t y = 1; y < (int64_t)volume.size(1); y++) {
            for (int64_t x = 1; x < (int64_t)volume.size(0); x++) {
                const double v0 = volume(x, y, z) / (double)USHRT_MAX;
                const double v1 = volume(x, y, z + 1) / (double)USHRT_MAX;
                if ((v0 < threshold) != (v1 < threshold)) {
                    rectangle[0] = cubes(x - 1, y - 1, z);
                    rectangle[1] = cubes(x, y - 1, z);
                    rectangle[2] = cubes(x - 1, y, z);
                    rectangle[3] = cubes(x, y, z);

                    for (int t = 0; t < 2; t++) {
                        uint32_t tri[3];
                        for (int k = 0; k < 3; k++) {
                            const Vec3 &v = (v0 > v1) ? rectangle[triindex[t][k]] : rectangle[triindex[t][3 - k - 1]];
                            if (uniqueVertices.count(v) == 0) {
                                uniqueVertices[v] = static_cast<uint32_t>(vertices->size());
                                vertices->push_back(v);
                            }
                            tri[k] = uniqueVertices[v];
                        }

                        if (tri[0] != tri[1] && tri[0] != tri[2] && tri[1] != tri[2]) {
                            indices->push_back(tri[0]);
                            indices->push_back(tri[1]);
                            indices->push_back(tri[2]);
                        }
                    }
                }
            }
            pbar.step();
        }
    }

    printf("#vert: %d\n", (int)vertices->size());
    printf("#face: %d\n", (int)indices->size() / 3);
}

// }}
