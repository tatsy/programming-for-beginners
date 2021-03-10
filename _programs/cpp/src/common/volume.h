#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <array>

#include "debug.h"

struct Volume {
    Volume() = default;

    Volume(uint64_t sizeX, uint64_t sizeY, uint64_t sizeZ) {
        sizes = { sizeX, sizeY, sizeZ };
        data = std::make_unique<uint16_t[]>(sizeX * sizeY * sizeZ);
    }

    Volume(const std::string &filename, int sizeX, int sizeY, int sizeZ)
        : Volume(sizeX, sizeY, sizeZ) {
        load(filename);
    }

    Volume(const Volume &other)
        : Volume(other.sizes[0], other.sizes[1], other.sizes[2]) {
        const auto totalSize = sizes[0] * sizes[1] * sizes[2];
        std::memcpy(data.get(), other.data.get(), sizeof(uint16_t) * totalSize);
    }

    Volume(Volume &&other) noexcept {
        sizes = other.sizes;
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
            swap(first.sizes, second.sizes);
            swap(first.data, second.data);
        }
    }

    uint16_t &operator()(int x, int y, int z) {
        return data[(z * sizes[1] + y) * sizes[0] + x];
    }

    uint16_t operator()(int x, int y, int z) const {
        return data[(z * sizes[1] + y) * sizes[0] + x];
    }

    uint64_t size(int i) const {
        if (i < 0 || i >= 3) {
            throw std::runtime_error("Dimension index out of bounds!");
        }
        return sizes[i];
    }

    void load(const std::string &filename) {
        // {{ NOT_IMPL_ERROR();
        std::ifstream reader(filename.c_str(), std::ios::in | std::ios::binary);
        for (uint64_t z = 0; z < sizes[2]; z++) {
            for (uint64_t y = 0; y < sizes[1]; y++) {
                uint16_t *ptr = data.get() + (z * sizes[1] + y) * sizes[0];
                reader.read((char*)ptr, sizeof(uint16_t) * sizes[0]);
            }
        }
        reader.close();
        // }}
    }

private:
    std::array<uint64_t, 3> sizes;
    std::unique_ptr<uint16_t[]> data = nullptr;
};
