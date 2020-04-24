#pragma once

class Vector3D {
public:
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    explicit Vector3D(double x) : x(x), y(x), z(x) {}
    explicit Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    bool operator==(const Vector3D &other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    bool operator!=(const Vector3D &other) const {
        return !this->operator==(other);
    }

    double x, y, z;
};

