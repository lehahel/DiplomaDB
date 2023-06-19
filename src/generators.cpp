#include "generators.h"

#include "graph.h"
#include "sql.h"
#include "utils.h"
#include <functional>
#include <vector>
#include <iostream>

namespace NGenerators {

namespace {

bool MakeGraphImpl(
    NGraph::IDatabaseGraph& graph,
    const std::vector<NGraph::TRowId>& vertexIds,
    NSQL::TErrorInfo* errorInfo,
    std::function<bool(const NGraph::TRowId&, const NGraph::TRowId&)> predicate = nullptr
) {
    static constexpr std::size_t batchSize = 10000;
    std::size_t step = 0;
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
            std::cout << "Pushing edges batch " << step++ << std::endl;
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

bool MakeTree(NGraph::IDatabaseGraph& graph, std::size_t depth, std::vector<NGraph::TRowId>& vertexIds, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    static constexpr std::size_t batchSize = 10000;
    std::size_t step = 0;
    // if (!graph.CreateVertices(((1ull << depth) - 1), "tree", errorInfo)) {
    //     return false;
    // }
    // auto optionalVertexIds = graph.GetRowIds(errorInfo);
    // if (!optionalVertexIds) {
    //     return false;
    // }
    vertexIds = NUtils::CreateUUIDs((1ull << depth) - 1);
    std::size_t i = 0;
    std::vector<NGraph::TEdge> edges;
    edges.reserve(batchSize);
    while (2 * i + 1 < vertexIds.size()) {
        NGraph::TEdge edge1;
        edge1.SetFrom(vertexIds[i]);
        edge1.SetTo(vertexIds[2 * i + 1]);
        edges.push_back(edge1);

        NGraph::TEdge edge2;
        edge2.SetFrom(vertexIds[i]);
        edge2.SetTo(vertexIds[2 * i + 2]);
        edges.push_back(edge2);

        ++i;

        if (edges.size() < batchSize) {
            continue;
        }
        std::cout << "Pushing edges batch " << step++ << std::endl;
        if (!graph.AddEdgesBatch(edges, errorInfo)) {
            return false;
        }
        edges.clear();
        edges.reserve(batchSize);
    }
    return graph.AddEdgesBatch(edges, errorInfo);
}

}
