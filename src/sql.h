#pragma once

#include <clickhouse/block.h>
#include <clickhouse/client.h>

#include <optional>
#include <iostream>

namespace NSQL {

struct TErrorInfo {
    std::string Text;
    void Fill(std::string_view sourceSession, std::string_view text);
};

bool ExecuteQuery(clickhouse::Client& client, const std::string& query, TErrorInfo* errorInfo = nullptr, std::string_view sourceSession = "NSQL::ExecuteQuery");

bool Insert(clickhouse::Client& client, const std::string& tableName, const clickhouse::Block& block, TErrorInfo* errorInfo = nullptr, std::string_view sourceSession = "NSQL::Insert");

template <class Column>
class SelectResult {
public:
    using DataType = std::decay_t<decltype(std::declval<Column>().At(0))>;

    std::vector<DataType>& Get() {
        return Data;
    }
    void Append(const DataType& item) {
        Data.push_back(item);
    }

private:
    std::vector<DataType> Data;
};

template <class ColumnType>
auto MakeDeserializeColumnCallback(SelectResult<ColumnType>& result) {
    return [&result] (const clickhouse::Block& block) -> void {
        if (block.GetColumnCount() == 0) {
            return;
        }
        auto column = block[0]->As<ColumnType>();
        for (std::size_t i = 0; i < column->Size(); ++i) {
            result.Append(column->At(i));
        }
    };
}

template <class Column>
std::optional<std::vector<typename SelectResult<Column>::DataType>> SelectColumn(clickhouse::Client& client, const std::string& query, TErrorInfo* errorInfo = nullptr, std::string_view sourceSession = "NSQL::Select") {
    auto result = SelectResult<Column>();
    auto callback = MakeDeserializeColumnCallback(result);
    // std::cout << "Query: " << query << std::endl;
    try {
        client.Select(query, callback);
    } catch (const std::exception& ex) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, ex.what());
        }
        return std::nullopt;
    }
    return std::make_optional(result.Get());
}

}
