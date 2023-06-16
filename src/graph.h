#pragma once

#include <clickhouse/base/uuid.h>
#include <clickhouse/client.h>

#include "macro.h"
#include "sql.h"

#include <memory>

namespace NClickHouse = clickhouse;

namespace NGraph {

using TRowId = NClickHouse::UUID;

class TEdge {
    FIELD(TRowId, From);
    FIELD(TRowId, To);
};

class IDatabaseGraph {
public:
    IDatabaseGraph(const NClickHouse::ClientOptions& opts)
        : ClickHouseClient(opts) {}

    virtual bool InitializeOrGet(const std::string& name, NSQL::TErrorInfo* errorInfo = nullptr) = 0;

    virtual bool AddEdgesBatch(const std::vector<TEdge>& edges, NSQL::TErrorInfo* errorInfo = nullptr) = 0;

    virtual bool AddEdge(const TEdge& edge, NSQL::TErrorInfo* errorInfo = nullptr);
    virtual std::optional<std::vector<TRowId>> GetAdjacent(const TRowId& vertexId, NSQL::TErrorInfo* errorInfo = nullptr) = 0;
    virtual std::optional<std::vector<TRowId>> GetRowIds(NSQL::TErrorInfo* errorInfo = nullptr) = 0;

    virtual std::string GetTypeName() = 0;

    virtual bool Drop(NSQL::TErrorInfo* errorInfo = nullptr) = 0;

    virtual ~IDatabaseGraph() {
    };

    const NClickHouse::Client& GetClickhouseClient() const;

private:
    FIELD(std::string, TableName);

protected:
    NClickHouse::Client ClickHouseClient;
};

class TAdjacencyListGraph : public IDatabaseGraph {
private:
    using TBase = IDatabaseGraph;

public:
    using TBase::TBase;

    static std::unique_ptr<IDatabaseGraph> Create(const NClickHouse::ClientOptions& opts) {
        return std::make_unique<TAdjacencyListGraph>(opts);
    }

    bool InitializeOrGet(const std::string& name, NSQL::TErrorInfo* errorInfo = nullptr) override;

    bool AddEdgesBatch(const std::vector<TEdge>& edges, NSQL::TErrorInfo* errorInfo = nullptr) override;
    std::optional<std::vector<TRowId>> GetAdjacent(const TRowId& vertexId, NSQL::TErrorInfo* errorInfo = nullptr) override;
    std::optional<std::vector<TRowId>> GetRowIds(NSQL::TErrorInfo* errorInfo = nullptr) override;

    bool Drop(NSQL::TErrorInfo* errorInfo = nullptr) override;

    std::string GetTypeName() override {
        return "adjacency_list_graph";
    }
};

namespace NAlgo {

bool DepthFirstSearch(const IDatabaseGraph& graph, const TRowId& startVertex);
bool BreadthFirstSearch(const IDatabaseGraph& graph, const TRowId& endVertex);

}

}
