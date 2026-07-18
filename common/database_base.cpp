#include <spdlog/spdlog.h>

#include "database_base.h"

DatabaseBase::DatabaseBase(const std::string& connection_string) : conn_(connection_string) {
    spdlog::info("Connected to database");
}

pqxx::connection& DatabaseBase::connection() { return conn_; }

std::vector<ActiveToken> DatabaseBase::fetch_active_tokens() {
    std::vector<ActiveToken> result;
    try {
        pqxx::work txn(conn_);
        auto rows = txn.exec(
            "SELECT id, stats_token FROM tokens "
            "WHERE NOW() BETWEEN activation AND expiration");
        for (const auto& row : rows) {
            result.push_back({row[0].as<int>(), row[1].as<std::string>()});
            spdlog::info(row[0].as<std::string>());
        }
        txn.commit();
    } catch (const std::exception& e) {
        spdlog::error("fetch_active_tokens failed: {}", e.what());
    }
    return result;
}