#include "sql.h"

#include <fmt/format.h>

namespace NSQL {

void TErrorInfo::Fill(std::string_view sourceSession, std::string_view text) {
    Text = fmt::format("{}: {}", sourceSession, text);
}

bool ExecuteQuery(clickhouse::Client& client, const std::string& query, TErrorInfo* errorInfo /* = nullptr */, std::string_view sourceSession /* = "ExecuteQuery" */) {
    try {
        client.Execute(query);
    } catch (const std::exception& e) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, e.what());
        }
        return false;
    }
    return true;
}

bool Insert(clickhouse::Client& client, const std::string& tableName, const clickhouse::Block& block, TErrorInfo* errorInfo /* = nullptr */, std::string_view sourceSession /* = "NSQL::Insert" */) {
    try {
        client.Insert(tableName, block);
    } catch (const std::exception& ex) {
        if (errorInfo) {
            errorInfo->Fill(sourceSession, ex.what());
        }
        return false;
    }
    return true;
}

}
