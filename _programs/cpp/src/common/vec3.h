#pragma once

#include <cmath>
#include <stdexcept>
#include <functional>

class Vec3 {
public:
    Vec3() : x(0.0), y(0.0), z(0.0) {}
    explicit Vec3(double x) : x(x), y(x), z(x) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    bool operator==(const Vec3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec3 &other) const {
        return !this->operator==(other);
    }

    Vec3 &operator+=(const Vec3 &other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 &operator-=(const Vec3 &other) {
        return this->operator+=(-other);
    }

    Vec3 &operator*=(const Vec3 &other) {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        return *this;
    }

    Vec3 &operator*=(double s) {
        return this->operator*=(Vec3(s));
    }

    Vec3 &operator/=(const Vec3 &other) {
        if (other.x == 0.0 || other.y == 0.0 || other.z == 0.0) {
            throw std::runtime_error("Zero division detected!");
        }
        return this->operator*=(Vec3(1.0 / other.x, 1.0 / other.y, 1.0 / other.z));
    }

    Vec3 &operator/=(double s) {
        return this->operator/=(Vec3(s));
    }

    double operator[](int i) const {
        if (i < 0 || i >= 3) {
            throw std::runtime_error("Vector index out of bounds!");
        }
        return (&x)[i];
    }

    double x, y, z;
};

// Basic arithmetics
inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
    Vec3 ret = v1;
    ret += v2;
    return ret;
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
    Vec3 ret = v1;
    ret -= v2;
    return ret;
}

inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2) {
    Vec3 ret = v1;
    ret *= v2;
    return ret;
}

inline Vec3 operator*(const Vec3 &v1, double s) {
    Vec3 ret = v1;
    ret *= s;
    return ret;
}

inline Vec3 operator*(double s, const Vec3 &v2) {
    Vec3 ret = v2;
    ret *= s;
    return ret;
}

inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
    Vec3 ret = v1;
    ret /= v2;
    return ret;
}

inline Vec3 operator/(const Vec3 &v1, double s) {
    Vec3 ret = v1;
    ret /= s;
    return ret;
}

// GLSL like vector arithmetic
inline double dot(const Vec3 &v1, const Vec3 &v2) {
    const Vec3 temp = v1 * v2;
    return temp.x + temp.y + temp.z;
}

inline Vec3 cross(const Vec3 &v1, const Vec3 &v2) {
    const double x = v1.y * v2.z - v1.z * v2.y;
    const double y = v1.z * v2.x - v1.x * v2.z;
    const double z = v1.x * v2.y - v1.y * v2.x;
    return Vec3(x, y, z);
}

inline double length(const Vec3 &v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3 &v) {
    return v / length(v);
}

// Hash
namespace std {

template <>
struct hash<Vec3> {
    std::size_t operator()(const Vec3& v) const {
        std::size_t h = 0;
        h = std::hash<double>()(v.x) ^ (h << 1);
        h = std::hash<double>()(v.y) ^ (h << 1);
        h = std::hash<double>()(v.z) ^ (h << 1);
        return h;
    }
};

}  // namespace std
