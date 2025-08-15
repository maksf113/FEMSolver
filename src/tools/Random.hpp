#pragma once
#include <random>
#include <chrono>

class Random {
public:
    // Callable interface: returns random double in [min, max]
    double operator()(double min, double max) const {
        return get(min, max);
    }

    // Static function interface
    static double get(double min, double max) {
        static thread_local std::mt19937 engine(seed());
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine);
    }

private:
    // Seed from high-resolution clock, only once
    static std::mt19937::result_type seed() {
        return static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count());
    }
};