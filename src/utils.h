#pragma once

#include <clickhouse/base/uuid.h>

#include <chrono>
#include <queue>

namespace NUtils {

bool GetBernoulliValue(double p);

std::uint64_t CreateRandomInt64();

clickhouse::UUID CreateUUID();

std::vector<clickhouse::UUID> CreateUUIDs(std::size_t size);

std::string GetRepresentation(clickhouse::UUID uuid);

std::string GetRepresentation2(clickhouse::UUID uuid);

std::string GetRepresentation(const std::vector<clickhouse::UUID>& uuids);

std::string PopAndGetRepresentation(std::queue<clickhouse::UUID>& uuids, std::size_t num);

class TTimer {
public:
    void Set();
    std::chrono::milliseconds GetElapsedTime() const;
public:
    std::chrono::system_clock::time_point StartTime;
};

}