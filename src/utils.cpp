#include "utils.h"
#include "clickhouse/base/uuid.h"

#include <chrono>
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

std::vector<clickhouse::UUID> CreateUUIDs(std::size_t size) {
    std::vector<clickhouse::UUID> result;
    result.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        result.push_back(CreateUUID());
    }
    return result;
}

std::string GetRepresentation(clickhouse::UUID uuid) {
    static constexpr std::string_view query = "toUUID(concat(leftPad(cast(hex(toUInt64({})), 'String'), 16, '0'), leftPad(cast(hex(toUInt64({})), 'String'), 16, '0')))";
    return fmt::format(query, uuid.first, uuid.second);
}

std::string GetRepresentation2(clickhouse::UUID uuid) {
    char firstHalf[17];
    char secondHalf[17];
    sprintf(firstHalf, "%016llx", uuid.first);
    sprintf(secondHalf, "%016llx", uuid.second);
    firstHalf[16] = '\0';
    secondHalf[16] = '\0';
    std::string result;
    result += "toUUID('";
    result += firstHalf;
    result += secondHalf;
    result += "')";
    return result;
}

std::string GetRepresentation(const std::vector<clickhouse::UUID>& uuids) {
    std::string result = "(";
    for (auto&& item : uuids) {
        result += GetRepresentation(item) + ",";
    }
    if (result.size() > 1) {
        result.pop_back();
    }
    result.push_back(')');
    return result;
}

std::string PopAndGetRepresentation(std::queue<clickhouse::UUID>& uuids, std::size_t num) {
    std::string result = "(";
    for (std::size_t i = 0; i < num; ++i) {
        result += GetRepresentation(uuids.front());
        uuids.pop();
    }
    if (result.size() > 1) {
        result.pop_back();
    }
    result.push_back(')');
    return result;
}

void TTimer::Set() {
    StartTime = std::chrono::system_clock::now();
}

std::chrono::milliseconds TTimer::GetElapsedTime() const {
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - StartTime);
}

}
