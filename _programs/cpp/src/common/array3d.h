#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ARRAY3D_H_
#define _ARRAY3D_H_

#include <array>
#include <vector>

template <typename T>
class Array3D {
public:
    Array3D() = default;
    Array3D(size_t sizeX, size_t sizeY, size_t sizeZ)
        : size_({ sizeX, sizeY, sizeZ })
        , data_() {
        data_.resize(length());
    }

    Array3D(const Array3D &other)
        : Array3D(other.size_[0], other.size_[1], other.size_[2]) {
        std::copy(other.data_.begin(), other.data_.end(), data_.begin());
    }

    Array3D(Array3D &&other) noexcept
        : size_(other.size_) {
        data_ = std::move(other.data_);
    }

    virtual ~Array3D() = default;

    Array3D &operator=(Array3D other) {
        swap(*this, other);
        return *this;
    }

    friend void swap(Array3D &first, Array3D &second) {
        using std::swap;
        if (&first != &second) {
            swap(first.size_, second.size_);
            swap(first.data_, second.data_);
        }
    }

    const T &operator()(size_t x, size_t y, size_t z) const {
        return data_[(z * size_[1] + y) * size_[0] + x];
    }

    T &operator()(size_t x, size_t y, size_t z) {
        return data_[(z * size_[1] + y) * size_[0] + x];
    }

    size_t size(int dim) const { return size_[dim]; }
    size_t length() const {
        size_t ret = 1;
        for (size_t s : size_) ret *= s;
        return ret;
    }

    std::array<size_t, 3> size_;
    std::vector<T> data_;
};

#endif  // _ARRAY3D_H_
