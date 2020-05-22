#pragma once

#include <chrono>
typedef std::chrono::time_point<std::chrono::system_clock> time_type;
inline time_type tick() {
    return std::chrono::system_clock::now();
}
inline double to_duration(time_type start, time_type end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
}

class Timer {
private:
    time_type _start;
    time_type _end;

public:
    Timer()
            : _start()
            , _end()
    {
    }

    void start() {
        _start = tick();
    }

    double stop() {
        _end = tick();
        return to_duration(_start, _end);
    }
};