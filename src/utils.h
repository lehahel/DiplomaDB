#pragma once

#include <clickhouse/base/uuid.h>

namespace NUtils {

bool GetBernoulliValue(double p);

std::uint64_t CreateRandomInt64();

clickhouse::UUID CreateUUID();

std::string GetRepresentation(clickhouse::UUID uuid);

}