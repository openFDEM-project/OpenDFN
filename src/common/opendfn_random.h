/******************************************************************************
 * OpenDFN random-number utilities.
 *
 * The implementation uses the C++ standard library so that OpenDFN does not
 * require an external random-number library at link time.
 ******************************************************************************/

#ifndef OPENDFN_RANDOM_H
#define OPENDFN_RANDOM_H

#include <cstdint>
#include <cstdlib>
#include <random>

namespace ns_common {

class OpenDFNRandom {
public:
    OpenDFNRandom() : engine_(readSeed()) {}

    double uniform() {
        return std::uniform_real_distribution<double>(0.0, 1.0)(engine_);
    }

    double gaussian(double sigma) {
        return std::normal_distribution<double>(0.0, sigma)(engine_);
    }

    double fisherF(double numeratorDegrees, double denominatorDegrees) {
        return std::fisher_f_distribution<double>(numeratorDegrees, denominatorDegrees)(engine_);
    }

private:
    static std::uint64_t readSeed() {
        const char* value = std::getenv("OPENDFN_RANDOM_SEED");
        if (value == nullptr || *value == '\0')
            return 0;

        char* end = nullptr;
        const auto parsed = std::strtoull(value, &end, 10);
        return (end != value && *end == '\0') ? static_cast<std::uint64_t>(parsed) : 0;
    }

    std::mt19937_64 engine_;
};

}  // namespace ns_common

#endif  // OPENDFN_RANDOM_H
