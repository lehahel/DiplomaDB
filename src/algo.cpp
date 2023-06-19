#include "algo.h"

#include "graph.h"
#include "macro.h"
#include "sql.h"
#include "utils.h"

#include <algorithm>
#include <clickhouse/base/uuid.h>
#include <clickhouse/client.h>

#include <exception>
#include <fmt/format.h>
#include <limits>
#include <stack>
#include <string_view>
#include <queue>
#include <iostream>

namespace NAlgo {

bool DepthFirstSearch(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, NSQL::TErrorInfo* errorInfo  /* = nullptr */) {
    auto rowIds = graph.GetRowIds(errorInfo);
    if (!rowIds) {
        return false;
    }
    std::vector<NGraph::TRowId> verticesToVisit;
    std::map<NGraph::TRowId, bool> isVisited;
    for (auto&& item : *rowIds) {
        isVisited.emplace(item, false);
    }
    verticesToVisit.push_back(startVertex);
    std::size_t step = 0;
    while (!verticesToVisit.empty()) {
        auto currentVertex = verticesToVisit.back();
        verticesToVisit.pop_back();
        if (isVisited[currentVertex]) {
            continue;
        }
        isVisited[currentVertex] = true;
        auto adj = graph.GetAdjacent(currentVertex, errorInfo);
        if (!adj) {
            return false;
        }
        for (auto&& item : *adj) {
            if (!isVisited[item]) {
                verticesToVisit.push_back(item);
            }
        }
    }
    return true;
}

bool BreadthFirstSearch(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    auto rowIds = graph.GetRowIds(errorInfo);
    if (!rowIds) {
        return false;
    }
    std::queue<NGraph::TRowId> verticesToVisit;
    std::map<NGraph::TRowId, bool> isVisited;
    for (auto&& item : *rowIds) {
        isVisited.emplace(item, false);
    }
    verticesToVisit.push(startVertex);
    while (!verticesToVisit.empty()) {
        auto currentVertex = verticesToVisit.front();
        verticesToVisit.pop();
        if (isVisited[currentVertex]) {
            continue;
        }
        isVisited[currentVertex] = true;
        auto adj = graph.GetAdjacent(currentVertex, errorInfo);
        if (!adj) {
            return false;
        }
        for (auto&& item : *adj) {
            if (!isVisited[item]) {
                verticesToVisit.push(item);
            }
        }
    }
    return true;
}

std::optional<std::vector<NGraph::TRowId>> FindPath(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, const NGraph::TRowId& endVertex, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    auto rowIds = graph.GetRowIds(errorInfo);
    if (!rowIds) {
        return std::nullopt;
    }
    std::queue<NGraph::TRowId> verticesToVisit;
    std::map<NGraph::TRowId, bool> isVisited;
    std::map<NGraph::TRowId, NGraph::TRowId> prev;
    for (auto&& item : *rowIds) {
        isVisited.emplace(item, false);
    }
    verticesToVisit.push(startVertex);
    while (!verticesToVisit.empty()) {
        auto currentVertex = verticesToVisit.front();
        if (currentVertex == endVertex) {
            break;
        }
        verticesToVisit.pop();
        if (isVisited[currentVertex]) {
            continue;
        }
        isVisited[currentVertex] = true;
        auto adj = graph.GetAdjacent(currentVertex, errorInfo);
        if (!adj) {
            return std::nullopt;
        }
        bool needBreak = false;
        for (auto&& item : *adj) {
            if (!isVisited[item]) {
                verticesToVisit.push(item);
                prev[item] = currentVertex;
            }
            if (item == endVertex) {
                needBreak = true;
                break;
            }
        }
        if (needBreak) {
            break;
        }
    }
    std::vector<NGraph::TRowId> path;
    NGraph::TRowId currentStep = endVertex;
    while (prev.find(currentStep) != prev.end()) {
        path.push_back(currentStep);
        currentStep = prev[currentStep];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::optional<bool> IsPathExistsVectored(NGraph::IDatabaseGraph& graph, const NGraph::TRowId& startVertex, const NGraph::TRowId& endVertex, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
    auto rowIds = graph.GetRowIds();
    if (!rowIds) {
        return std::nullopt;
    }
    std::queue<NGraph::TRowId> verticesToVisit;
    std::map<NGraph::TRowId, bool> isVisited;
    for (auto&& item : *rowIds) {
        isVisited.emplace(item, false);
    }
    verticesToVisit.push(startVertex);
    while (!verticesToVisit.empty()) {
        std::string queryArray = "has([";
        std::size_t count = 0;
        for (std::size_t i = 0; i < 6000 && verticesToVisit.size() > 0; ++i) {
            if (verticesToVisit.front() == endVertex) {
                return true;
            }
            isVisited[verticesToVisit.front()] = true;
            queryArray += NUtils::GetRepresentation2(verticesToVisit.front()) + ",";
            verticesToVisit.pop();
            ++count;
        }
        if (count > 0) {
            queryArray.pop_back();
        }
        queryArray += "], from) = 1";
        auto adj = graph.GetToByCustomCondition(queryArray, errorInfo);
        if (!adj) {
            return std::nullopt;
        }
        bool needBreak = false;
        for (auto&& item : *adj) {
            if (!isVisited[item]) {
                verticesToVisit.push(item);
            }
            if (item == endVertex) {
                return true;
            }
        }
    }
    return false;
}

// TOptionalPath FindShortestPath(NGraph::IDatabaseGraph& graph, NClickHouse::UUID startVertexId, NSQL::TErrorInfo* errorInfo /* = nullptr */) {
//     static constexpr std::string_view sourceSession = "FindShortestPath";
//     const auto& clickHouseClient = graph.GetClickhouseClient();
//     std::unordered_map<NClickHouse::UUID, std::uint64_t> distances;
//     std::unordered_map<NClickHouse::UUID, bool> visited;
//     auto vertexIds = graph.GetRowIds(errorInfo);
//     if (!vertexIds) {
//         return std::nullopt;
//     }
//     for (auto&& id : *vertexIds) {
//         distances[id] = std::numeric_limits<std::uint64_t>::max();
//         visited[id] = false;
//     }

// }

}
