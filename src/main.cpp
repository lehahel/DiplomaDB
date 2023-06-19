#include <iostream>
#include <clickhouse/client.h>
#include <stdexcept>
#include <unordered_map>

#include "algo.h"
#include "clickhouse/base/uuid.h"
#include "generators.h"
#include "graph.h"
#include "utils.h"

using namespace clickhouse;

int main()
{
    NSQL::TErrorInfo errorInfo;
    NClickHouse::ClientOptions opts;
    opts.SetHost("localhost");
    opts.SetPort(9000);
    auto graph = NGraph::TAdjacencyListGraph::Create(opts);
    if (!graph->InitializeOrGet("test_graph", &errorInfo)) {
        throw std::runtime_error(errorInfo.Text);
    }

    std::size_t treeDepth{0};
    std::cin >> treeDepth;

    std::vector<NGraph::TRowId> vertexIds;
    if (!NGenerators::MakeTree(*graph, treeDepth, vertexIds, &errorInfo)) {
        throw std::runtime_error(errorInfo.Text);
    }

    {
        std::cout << "IsPathExistsVectored started" << std::endl;
        NUtils::TTimer timer;
        timer.Set();
        if (!NAlgo::IsPathExistsVectored(*graph, vertexIds.front(), vertexIds.back(), &errorInfo)) {
            throw std::runtime_error(errorInfo.Text);
        }
        std::cout << "IsPathExistsVectored time: " << timer.GetElapsedTime().count() << " ms" << std::endl;
    }

    if (!graph->Drop(&errorInfo)) {
        throw std::runtime_error(errorInfo.Text);
    }

    // std::size_t graphSize;
    // double probability;
    // std::cin >> graphSize >> probability;

    // auto vertexIds = NUtils::CreateUUIDs(graphSize);

    // if (!NGenerators::MakeRandomGraph(*graph, vertexIds, probability, &errorInfo)) {
    //     throw std::runtime_error(errorInfo.Text);
    // }

    // {
    //     std::cout << "DFS started" << std::endl;
    //     NUtils::TTimer timer;
    //     timer.Set();
    //     if (!NAlgo::DepthFirstSearch(*graph, vertexIds.front(), &errorInfo)) {
    //         throw std::runtime_error(errorInfo.Text);
    //     }
    //     std::cout << "DFS time: " << timer.GetElapsedTime().count() << " ms" << std::endl;
    // }

    // {
    //     std::cout << "FindPath started" << std::endl;
    //     NUtils::TTimer timer;
    //     timer.Set();
    //     if (!NAlgo::FindPath(*graph, vertexIds.front(), vertexIds.back(), &errorInfo)) {
    //         throw std::runtime_error(errorInfo.Text);
    //     }
    //     std::cout << "FindPath time: " << timer.GetElapsedTime().count() << " ms" << std::endl;
    // }

    // auto vertexIds = NUtils::CreateUUIDs(10);
    // std::map<NClickHouse::UUID, std::size_t> uuidToNum;
    // for (std::size_t i = 0; i < vertexIds.size(); ++i) {
    //     uuidToNum[vertexIds[i]] = i;
    // }
    // if (!NGenerators::MakeRandomGraph(*graph, vertexIds, 0.1, &errorInfo)) {
    //     throw std::runtime_error(errorInfo.Text);
    // }

    // for (auto&& item : vertexIds) {
    //     auto adj = graph->GetAdjacent(item, &errorInfo);
    //     if (!adj) {
    //         throw std::runtime_error(errorInfo.Text);
    //     }
    //     auto num = uuidToNum[item];
    //     std::cout << num << ": (";
    //     for (auto&& v : *adj) {
    //         auto vnum = uuidToNum[v];
    //         std::cout << vnum << " ";
    //     }
    //     std::cout << ")" << std::endl;
    // }

    return 0;
}