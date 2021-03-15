#pragma once

#include <string>

class filepath {
public:
    filepath(const std::string &path = "") : path_{path} {
        cleanup();
    }

    filepath(const char* path = "") : path_{path} {
        cleanup();
    }

    filepath operator+(const filepath &other) const {
        return filepath(this->string() + other.string());
    }

    filepath operator/(const filepath &other) const {
        return filepath(this->string() + "/" + other.string());
    }

    //! Basename
    filepath basename() const {
        const int pos = path_.find_last_of("/");
        return path_.substr(pos + 1);
    }

    //! Directory name
    filepath dirname() const {
        const int pos = path_.find_last_of("/");
        return path_.substr(0, pos);
    }

    //! Basename without extension
    filepath stem() const {
        const std::string base = basename().string();
        const int pos = base.find_first_of(".");
        return base.substr(0, pos);
    }

    //! Extension of file path
    filepath suffix() const {
        const std::string base = basename().string();
        const int pos = base.find_first_of(".");
        return base.substr(pos + 1);
    }

    //! Internal path string
    std::string string() const {
        return path_;
    }

private:
    void cleanup() {
        size_t pos = -1;
        while ((pos = path_.find_first_of("\\")) != std::string::npos) {
            path_.replace(pos, 1, "/");
        }
    }

    std::string path_;
};