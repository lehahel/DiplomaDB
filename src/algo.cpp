#include "algo.h"

#include "graph.h"
#include "macro.h"
#include "utils.h"

#include <clickhouse/base/uuid.h>
#include <clickhouse/client.h>

#include <exception>
#include <fmt/format.h>
#include <limits>
#include <string_view>

namespace NAlgo {

namespace {

constexpr std::string_view createVisitedTableQuery = "CREATE TABLE default.{} (id UUID, vertex_id UUID, visited BOOL) ENGINE = Memory";
constexpr std::string_view selectVisitedQuery = "SELECT vertex_id, visited from {} WHERE vertex_id = {}";

class TVisited {
public:
    TVisited(clickhouse::Client* client)
        : ClickHouseClient(client) {}

    bool Initialize(std::string_view name, NSQL::TErrorInfo* errorInfo = nullptr) {
        static constexpr std::string_view sourceSession = "TVisited::Intialize";
        auto tableName = fmt::format("{}_{}", name, NUtils::CreateRandomInt64());
        auto sqlQuery = fmt::format(createVisitedTableQuery, tableName);
        SetTableName(tableName);
        return NSQL::ExecuteQuery(*ClickHouseClient, sqlQuery);
    }

    std::optional<bool> operator[](clickhouse::UUID vertexId) {
        auto sqlQuery = fmt::format(selectVisitedQuery, GetTableName(), NUtils::GetRepresentation(vertexId));

    }

private:
    FIELD(std::string, TableName);
    clickhouse::Client* ClickHouseClient;
};

}

TOptionalPath FindShortestPath(NGraph::IDatabaseGraph& graph, NClickHouse::UUID startVertexId, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr std::string_view sourceSession = "FindShortestPath";
    const auto& clickHouseClient = graph.GetClickhouseClient();
    std::unordered_map<NClickHouse::UUID, std::uint64_t> distances;
    std::unordered_map<NClickHouse::UUID, bool> visited;
    auto vertexIds = graph.GetRowIds(errorInfo);
    if (!vertexIds) {
        return std::nullopt;
    }
    for (auto&& id : *vertexIds) {
        distances[id] = std::numeric_limits<std::uint64_t>::max();
        visited[id] = false;
    }

}

}
