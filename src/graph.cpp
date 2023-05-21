#include "graph.h"
#include "clickhouse/query.h"

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
#include <random>
#include <stdexcept>
#include <iostream>

namespace NGraph {

void TErrorInfo::Fill(std::string_view sourceSession, std::string_view text) {
    Text = fmt::format("{}: {}", sourceSession, text);
}

namespace {
    constexpr auto createEdgesTableQuery = "CREATE TABLE IF NOT EXISTS default.{} (id UUID, from UUID, to UUID) ENGINE = {}";
    constexpr auto selectAdjacentEdgesQuery = "SELECT to FROM {} WHERE from = UUIDNumToString(toFixedString('{}', 16))";

    inline std::string MakeTableName(std::string_view prefix, std::string_view name) {
        return fmt::format("{}_{}", prefix, name);
    }

    std::uint64_t CreateRandomInt64() {
        std::mt19937_64 engine(std::random_device{}());
        std::uniform_int_distribution<uint64_t> distribution;
        return distribution(engine);
    }

    NClickHouse::UUID CreateUUID() {
        NClickHouse::UUID result;
        result.first = CreateRandomInt64();
        result.second = CreateRandomInt64();
        return result;
    }
}

bool IDatabaseGraph::AddEdge(const TEdge& edge, TErrorInfo* errorInfo /* = nullptr */) {
    return AddEdgesBatch({ edge }, errorInfo);
}

bool TAdjacencyListGraph::InitializeOrGet(const std::string& name, TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::InitializeOrGet";
    static constexpr auto engine = "Memory";
    SetTableName(MakeTableName(GetTypeName(), name));

    auto sqlQuery = fmt::format(createEdgesTableQuery, GetTableName(), engine);
    try {
        ClickHouseClient.Execute(sqlQuery);
    } catch (const std::exception& ex) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, ex.what());
        }
        return false;
    }
    return true;
}

bool TAdjacencyListGraph::AddEdgesBatch(const std::vector<TEdge>& edges, TErrorInfo* errorInfo) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::AddEdgesBatch";
    auto idColumn = std::make_shared<NClickHouse::ColumnUUID>();
    auto fromColumn = std::make_shared<NClickHouse::ColumnUUID>();
    auto toColumn = std::make_shared<NClickHouse::ColumnUUID>();
    for (auto&& edge : edges) {
        std::cout << "New edge from " << edge.GetFrom().first << " " << edge.GetFrom().second << " to " << edge.GetTo().first << " " << edge.GetTo().second << std::endl;
        idColumn->Append(CreateUUID());
        fromColumn->Append(edge.GetFrom());
        toColumn->Append(edge.GetTo());
    }
    NClickHouse::Block block;
    block.AppendColumn("id", idColumn);
    block.AppendColumn("from", fromColumn);
    block.AppendColumn("to", toColumn);
    try {
        ClickHouseClient.Insert(GetTableName(), block);
    } catch (const std::exception& ex) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, ex.what());
        }
        return false;
    }
    return true;
}

std::optional<std::vector<TRowId>> TAdjacencyListGraph::GetAdjacent(const TRowId& vertexId, TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr auto sourceSession = "TAdjacencyListGraph::GetAdjacent";
    auto result = std::make_optional<std::vector<TRowId>>();
    NClickHouse::QuerySettings query;
    const char* vertexIdBytes = reinterpret_cast<const char*>(&vertexId);
    std::string_view vertexIdCasted(vertexIdBytes, sizeof(vertexId));
    auto sqlQuery = fmt::format(selectAdjacentEdgesQuery, GetTableName(), vertexIdCasted);
    try {
        ClickHouseClient.Select(sqlQuery, [&result] (const NClickHouse::Block& block) -> void {
            if (block.GetColumnCount() != 1ul) {
                auto text = fmt::format("Invalid column number got (expected 1, got {}).", block.GetColumnCount());
                throw std::runtime_error(text);
            }
            auto column = block[0]->As<NClickHouse::ColumnUUID>();
            for (std::size_t i = 0; i < column->Size(); ++i) {
                result->push_back(column->At(i));
            }
        });
    } catch (const std::exception& ex) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, ex.what());
        }
        return std::nullopt;
    }
    return result;
}

}
