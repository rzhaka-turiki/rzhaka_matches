#include "database.h"

#include <stdexcept>

Database::Database(const std::string& connection_string) : conn_(connection_string) {
    if (!conn_.is_open()) throw std::runtime_error("Failed to connect to database");
}

pqxx::connection& Database::connection() { return conn_; }

std::vector<ActiveToken> Database::fetch_active_tokens() {
    std::vector<ActiveToken> result;
    pqxx::work txn(conn_);
    auto rows = txn.exec("SELECT id, stats_token FROM tokens WHERE NOW() BETWEEN activation AND expiration");
    for (const auto& row : rows) {
        result.push_back({row[0].as<int>(), row[1].as<std::string>()});
    }
    txn.commit();
    return result;
}