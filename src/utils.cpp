#include "utils.h"

#include <random>

#include <fmt/format.h>

namespace NUtils {

namespace {

template <class Distribution, class... Args>
auto GetRandomValue(Args&&... args) {
    std::mt19937_64 engine(std::random_device{}());
    Distribution distribution(std::forward<Args>(args)...);
    return distribution(engine);
}

}

bool GetBernoulliValue(double p) {
    return GetRandomValue<std::bernoulli_distribution>(p);
}

std::uint64_t CreateRandomInt64() {
    return GetRandomValue<std::uniform_int_distribution<std::uint64_t>>();
}

clickhouse::UUID CreateUUID() {
    clickhouse::UUID result;
    result.first = CreateRandomInt64();
    result.second = CreateRandomInt64();
    return result;
}

std::string GetRepresentation(clickhouse::UUID uuid) {
    static constexpr std::string_view query = "toUUID(concat(leftPad(cast(hex(toUInt64({})), 'String'), 16, '0'), leftPad(cast(hex(toUInt64({})), 'String'), 16, '0')))";
    return fmt::format(query, uuid.first, uuid.second);
}

}
