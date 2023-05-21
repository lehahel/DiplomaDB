#include <iostream>
#include <clickhouse/client.h>
#include <stdexcept>

#include "graph.h"

using namespace clickhouse;

namespace GraphCH {

} // namespace GraphCH

int main()
{
    NGraph::TErrorInfo errorInfo;
    NClickHouse::ClientOptions opts;
    opts.SetHost("localhost");
    opts.SetPort(9000);
    auto graph = NGraph::TAdjacencyListGraph::Create(opts);
    if (!graph->InitializeOrGet("test_graph", &errorInfo)) {
        throw std::runtime_error(errorInfo.Text);
    }

    NGraph::TRowId vertexId1;
    vertexId1.first = 1;
    vertexId1.second = 1;

    NGraph::TRowId vertexId2;
    vertexId2.first = 2;
    vertexId2.second = 2;

    NGraph::TRowId vertexId3;
    vertexId3.first = 3;
    vertexId3.second = 3;

    NGraph::TRowId vertexId4;
    vertexId4.first = 4;
    vertexId4.second = 4;

    NGraph::TEdge edge1;
    edge1.SetFrom(vertexId1);
    edge1.SetTo(vertexId2);

    NGraph::TEdge edge2;
    edge1.SetFrom(vertexId1);
    edge1.SetTo(vertexId3);

    NGraph::TEdge edge3;
    edge1.SetFrom(vertexId1);
    edge1.SetTo(vertexId4);

    NGraph::TEdge edge4;
    edge1.SetFrom(vertexId3);
    edge1.SetTo(vertexId4);

    NGraph::TEdge edge5;
    edge1.SetFrom(vertexId2);
    edge1.SetTo(vertexId1);

    std::vector edges{edge1, edge2, edge3, edge4, edge5};
    if (!graph->AddEdgesBatch(edges, &errorInfo)) {
        throw std::runtime_error(errorInfo.Text);
    }

    auto adj = graph->GetAdjacent(vertexId1, &errorInfo);
    if (!adj) {
        throw std::runtime_error(errorInfo.Text);
    }

    for (auto&& item : *adj) {
        std::cout << item.first << " " << item.second << std::endl;
    }

    // /// Initialize client connection.
    // Client client(ClientOptions().SetHost("localhost").SetPort(9000));

    // /// Create a table.
    // client.Execute("CREATE TABLE IF NOT EXISTS default.numbers (id UInt64, name String) ENGINE = Memory");

    // /// Insert some values.
    // {
    //     Block block;

    //     auto id = std::make_shared<ColumnUInt64>();
    //     id->Append(1);
    //     id->Append(7);

    //     auto name = std::make_shared<ColumnString>();
    //     name->Append("one");
    //     name->Append("seven");

    //     block.AppendColumn("id"  , id);
    //     block.AppendColumn("name", name);

    //     client.Insert("default.numbers", block);
    // }

    // /// Select values inserted in the previous step.
    // client.Select("SELECT id, name FROM default.numbers", [] (const Block& block)
    //     {
    //         for (size_t i = 0; i < block.GetRowCount(); ++i) {
    //             std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
    //                       << block[1]->As<ColumnString>()->At(i) << "\n";
    //         }
    //     }
    // );

    // /// Delete table.
    // client.Execute("DROP TABLE default.numbers");

    return 0;
}