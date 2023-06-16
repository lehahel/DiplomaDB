#pragma once

#include "graph.h"

namespace NGenerators {

bool MakeFullGraph(NGraph::IDatabaseGraph& graph, const std::vector<NClickHouse::UUID>& vertexIds, NSQL::TErrorInfo* errorInfo = nullptr);

bool MakeRandomGraph(NGraph::IDatabaseGraph& graph, const std::vector<NGraph::TRowId>& vertexIds, double probability, NSQL::TErrorInfo* errorInfo = nullptr);

}
