#include "graph.h"

#include "sql.h"
#include "utils.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include <clickhouse/base/uuid.h>
#include <clickhouse/block.h>
#include <clickhouse/columns/uuid.h>

#include <exception>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace NGraph {

namespace {
    constexpr std::string_view createVerticesQuery = "CREATE TABLE IF NOT EXISTS default.{} (id UUID, value TEXT) ENGINE = {} PRIMARY KEY id";
    constexpr std::string_view createEdgesTableQuery = "CREATE TABLE IF NOT EXISTS default.{} (id UUID, from UUID, to UUID) ENGINE = {} ORDER BY from";
    constexpr std::string_view selectAdjacentEdgesQuery = "SELECT to FROM default.{} WHERE from = {}";
    constexpr std::string_view selectToByCustomConditionQuery = "SELECT to FROM default.{0} WHERE {1}";
    constexpr std::string_view dropTableQuery = "DROP TABLE {}";
    constexpr std::string_view selectVerticesQuery = "SELECT DISTINCT from AS vertex_id FROM default.{0} UNION DISTINCT SELECT DISTINCT to AS vertex_id FROM default.{0}";

    inline std::string MakeTableName(std::string_view prefix, std::string_view name) {
        return fmt::format("{}_{}", prefix, name);
    }
}

bool IDatabaseGraph::AddEdge(const TEdge& edge, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    return AddEdgesBatch({ edge }, errorInfo);
}

bool IDatabaseGraph::CreateVertices(const std::size_t size, std::string_view tableName, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr std::string_view sourceSession = "IDatabaseGraph::CreateVertices";
    static constexpr std::string_view engine = "MergeTree";
    SetVerticesTableName(MakeTableName(GetTypeName(), tableName));
    auto sqlQuery = fmt::format(createVerticesQuery, GetVerticesTableName(), engine);
    return NSQL::ExecuteQuery(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

bool TAdjacencyListGraph::InitializeOrGet(const std::string& name, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr std::string_view sourceSession = "TAdjacencyListGraph::InitializeOrGet";
    static constexpr std::string_view engine = "MergeTree";
    SetEdgesTableName(MakeTableName(GetTypeName(), name));
    auto sqlQuery = fmt::format(createEdgesTableQuery, GetEdgesTableName(), engine);
    return NSQL::ExecuteQuery(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

bool TAdjacencyListGraph::Drop(NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr std::string_view sourceSession = "TAdjacencyListGraph::Drop";
    auto sqlQuery = fmt::format(dropTableQuery, GetEdgesTableName());
    return NSQL::ExecuteQuery(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

bool TAdjacencyListGraph::AddEdgesBatch(const std::vector<TEdge>& edges, NSQL::TErrorInfo* errorInfo) {
    if (edges.empty()) {
        return true;
    }
    static constexpr auto sourceSession = "TAdjacencyListGraph::AddEdgesBatch";
    auto idColumn = std::make_shared<NClickHouse::ColumnUUID>();
    auto fromColumn = std::make_shared<NClickHouse::ColumnUUID>();
    auto toColumn = std::make_shared<NClickHouse::ColumnUUID>();
    for (auto&& edge : edges) {
        // std::cout << "New edge from " << edge.GetFrom().first << " " << edge.GetFrom().second << " to " << edge.GetTo().first << " " << edge.GetTo().second << std::endl;
        idColumn->Append(NUtils::CreateUUID());
        fromColumn->Append(edge.GetFrom());
        toColumn->Append(edge.GetTo());
    }
    NClickHouse::Block block;
    block.AppendColumn("id", idColumn);
    block.AppendColumn("from", fromColumn);
    block.AppendColumn("to", toColumn);
    return NSQL::Insert(ClickHouseClient, GetEdgesTableName(), block, errorInfo, sourceSession);
}

std::optional<std::vector<TRowId>> TAdjacencyListGraph::GetRowIds(NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::GetRowIds";
    auto sqlQuery = fmt::format(selectVerticesQuery, GetEdgesTableName());
    return NSQL::SelectColumn<NClickHouse::ColumnUUID>(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

std::optional<std::vector<TRowId>> TAdjacencyListGraph::GetAdjacent(const TRowId& vertexId, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::GetAdjacent";
    auto sqlQuery = fmt::format(selectAdjacentEdgesQuery, GetEdgesTableName(), NUtils::GetRepresentation(vertexId));
    return NSQL::SelectColumn<NClickHouse::ColumnUUID>(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

std::optional<std::vector<TRowId>> TAdjacencyListGraph::GetToByCustomCondition(std::string_view query, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::GetToByCustomCondition";
    auto sqlQuery = fmt::format(selectToByCustomConditionQuery, GetEdgesTableName(), query);
    return NSQL::SelectColumn<NClickHouse::ColumnUUID>(ClickHouseClient, sqlQuery, errorInfo, sourceSession);
}

}
