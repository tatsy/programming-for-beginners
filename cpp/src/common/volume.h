#pragma once

#include <cstdint>
#include <memory>

#include "debug.h"

struct Volume {
    Volume() = default;

    Volume(int sizeX, int sizeY, int sizeZ) {
        sizes[0] = sizeX;
        sizes[1] = sizeY;
        sizes[2] = sizeZ;
        data = std::make_unique<uint16_t>(sizeX * sizeY * sizeZ);
    }

    Volume(const std::string &filename, int sizeX, int sizeY, int sizeZ)
        : Volume(sizeX, sizeY, sizeZ) {
        NOT_IMPL_ERROR();
    }

    Volume(const Volume &other)
        : Volume(other.sizes[0], other.sizes[1], other.sizes[2]) {
        const auto totalSize = sizes[0] * sizes[1] * sizes[2];
        std::memcpy(data.get(), other.data.get(), sizeof(uint16_t) * totalSize);
    }

    Volume(Volume &&other) noexcept
        : Volume(other.sizes[0], other.sizes[1], other.sizes[2]) {
        data = std::move(other.data);
    }

    virtual ~Volume() = default;

    Volume &operator=(Volume other) {
        swap(*this, other);
        return *this;
    }

    friend void swap(Volume &first, Volume &second) {
        using std::swap;
        if (&first != &second) {
            swap(first.sizes[0], second.sizes[0]);
            swap(first.sizes[1], second.sizes[1]);
            swap(first.sizes[2], second.sizes[2]);
            swap(first.data, second.data);
        }
    }

    uint16_t &operator()(int x, int y, int z) {
        NOT_IMPL_ERROR();
    }

    uint16_t operator()(int x, int y, int z) const {
        NOT_IMPL_ERROR();
    }

    uint64_t size(int i) {
        if (i < 0 || i >= 3) {
            throw std::runtime_error("Dimension index out of bounds!");
        }
        return sizes[i];
    }

    void load(const std::string &filename) {
        NOT_IMPL_ERROR();
    }

private:
    uint64_t sizes[3] = {0};
    std::unique_ptr<uint16_t> data = nullptr;
};
