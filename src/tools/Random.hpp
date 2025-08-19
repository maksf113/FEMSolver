#pragma once
#include <random>
#include <type_traits>

class Random
{
public:
    // Callable interface: returns random T in [min, max]
    template <typename T>
    T operator()(T min, T max) const
    {
        return get(min, max);
    }

    // Static function interface
    template <typename T>
    static T get(T min, T max)
    {
        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(getEngine());
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(getEngine());
        }
        else
        {
            static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                "Random::get() only supports integral and floating point types\n");
            return T{};
        }
    }

private:
    // lazily seeded random number engine
    static std::mt19937& getEngine()
    {
        static thread_local std::mt19937 engine(seed());
        return engine;
    }
    // high quality seed
    static std::mt19937::result_type seed()
    {
        std::random_device rd;
        return rd();
    }
};
