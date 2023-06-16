#include "generators.h"

#include "graph.h"
#include "sql.h"
#include "utils.h"
#include <functional>
#include <vector>

namespace NGenerators {

namespace {

bool MakeGraphImpl(
    NGraph::IDatabaseGraph& graph,
    const std::vector<NGraph::TRowId>& vertexIds,
    NSQL::TErrorInfo* errorInfo,
    std::function<bool(const NGraph::TRowId&, const NGraph::TRowId&)> predicate = nullptr
) {
    static constexpr std::size_t batchSize = 1000;
    std::vector<NGraph::TEdge> edges;
    edges.reserve(batchSize);
    for (auto&& i : vertexIds) {
        for (auto&& j : vertexIds) {
            if (i == j) {
                continue;
            }
            if (predicate && !predicate(i, j)) {
                continue;
            }
            NGraph::TEdge edge;
            edge.SetFrom(i);
            edge.SetTo(j);
            edges.push_back(edge);
            if (edges.size() < batchSize) {
                continue;
            }
            if (!graph.AddEdgesBatch(edges, errorInfo)) {
                return false;
            }
            edges.clear();
            edges.reserve(batchSize);
        }
    }
    return graph.AddEdgesBatch(edges, errorInfo);
}

}

bool MakeFullGraph(NGraph::IDatabaseGraph& graph, const std::vector<NGraph::TRowId>& vertexIds, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    return MakeGraphImpl(graph, vertexIds, errorInfo);
}

bool MakeRandomGraph(NGraph::IDatabaseGraph& graph, const std::vector<NGraph::TRowId>& vertexIds, double probability, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    return MakeGraphImpl(graph, vertexIds, errorInfo, [probability](auto&& x, auto&& y) { return NUtils::GetBernoulliValue(probability); });
}

}
