#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <Eigen/Core>

#include "common/vec3.h"
#include "icp.h"

// Load OFF mesh file (in this program, the file stores only point cloud)
void read_off(const std::string &filename, std::vector<Vec3> *positions, std::vector<Vec3> *normals = nullptr) {
    std::ifstream reader(filename.c_str(), std::ios::in);
    if (reader.fail()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    std::stringstream ss;

    // Magic word
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

    reader.close();
}

void write_off(const std::string &filename, const std::vector<Vec3> &positions, const std::vector<Vec3> &normals = std::vector<Vec3>()) {
    std::ofstream writer(filename.c_str(), std::ios::out);
    if (writer.fail()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Magic word
    if (normals.empty()) {
        writer << "OFF" << std::endl;
    } else {
        if (positions.size() != normals.size()) {
            throw std::runtime_error("#pos and #norm do not match!");
        }
        writer << "NOFF" << std::endl;
    }

    const int nVerts = positions.size();
    const int nFaces = 0;
    const int nEdges = 0;
    writer << nVerts << " " << nFaces << " " << nEdges << std::endl;

    for (int i = 0; i < nVerts; i++) {
        const Vec3 &p = positions[i];
        writer << p.x << " " << p.y << " " << p.z;
        if (!normals.empty()) {
            const Vec3 &n = normals[i];
            writer << " " << n.x << " " << n.y << " " << n.z;
        }
        writer << std::endl;
    }

    writer.close();
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        fprintf(stderr, "[ USAGE ] icp_exe [ *.off file ] [ *.off file ] [ iteration ] [ tolerance ]\n");
        std::exit(1);
    }

    const int nIterations = argc > 3 ? atoi(argv[3]) : 100;
    const double tolerance = argc > 4 ? atof(argv[4]) : 1.0e-4;

    // Load point cloud data
    std::vector<Vec3> pos0;
    std::vector<Vec3> norm0;
    std::vector<Vec3> pos1;
    read_off(argv[1], &pos0, &norm0);
    read_off(argv[2], &pos1);
    printf("PCL #0: %ld points\n", pos0.size());
    printf("PCL #1: %ld points\n", pos1.size());

    // Point-to-point ICP
    for (int it = 0; it < nIterations; it++) {
        Eigen::MatrixXd R(3, 3);
        Eigen::VectorXd t(3);
        // point2pointICP(pos0, pos1, &R, &t);
        point2planeICP(pos0, norm0, pos1, &R, &t);

        // Apply rigid transformation
        for (int i = 0; i < (int) pos1.size(); i++) {
            Eigen::VectorXd v(3);
            v << pos1[i].x, pos1[i].y, pos1[i].z;
            Eigen::VectorXd u = R * v + t;
            pos1[i] = Vec3(u(0), u(1), u(2));
        }

        // Check current error
        const double error = t.norm() + (R - Eigen::MatrixXd::Identity(3, 3)).norm();

        // Report
        printf("*** %d iteration ***\n", it + 1);
        printf("R = \n");
        std::cout << R << std::endl;
        printf("t = \n");
        std::cout << t << std::endl;
        printf("error = %f\n", error);
        printf("\n");

        if (error < tolerance) {
            break;
        }
    }

    // Write as *.off
    write_off("output.off", pos1);
}