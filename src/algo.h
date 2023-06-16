#pragma once

#include "graph.h"
#include "sql.h"

namespace NAlgo {

using TPath = std::vector<NGraph::TRowId>;
using TOptionalPath = std::optional<TPath>;

TOptionalPath FindShortestPath(NGraph::IDatabaseGraph& graph, NClickHouse::UUID startVertexId, NSQL::TErrorInfo* errorInfo = nullptr);

}
