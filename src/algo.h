#pragma once

#include "graph.h"
#include "sql.h"

namespace NAlgo {

using TPath = std::vector<NGraph::TRowId>;
using TOptionalPath = std::optional<TPath>;

bool DepthFirstSearch(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, NSQL::TErrorInfo* errorInfo = nullptr);

bool BreadthFirstSearch(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& endVertex, NSQL::TErrorInfo* errorInfo = nullptr);

std::optional<std::vector<NGraph::TRowId>> FindPath(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, const NGraph::TRowId& endVertex, NSQL::TErrorInfo* errorInfo = nullptr);

std::optional<bool> IsPathExistsVectored(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, const NGraph::TRowId& endVertex, NSQL::TErrorInfo* errorInfo = nullptr);

TOptionalPath FindShortestPath(NGraph::IDatabaseGraph& graph, NClickHouse::UUID startVertexId, NSQL::TErrorInfo* errorInfo = nullptr);


}
