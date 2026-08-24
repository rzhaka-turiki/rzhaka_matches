#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>

struct ActiveToken {
    int id;
    std::string token;
};

class DatabaseBase {
protected:
    pqxx::connection conn_;

public:
    explicit DatabaseBase(const std::string& connection_string);
    virtual ~DatabaseBase() = default;

    pqxx::connection& connection();
    std::vector<ActiveToken> fetch_active_tokens();
};