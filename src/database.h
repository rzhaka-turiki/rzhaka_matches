#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>

struct ActiveToken {
    int id;
    std::string token;
};

class Database {
private:
    pqxx::connection conn_;

public:
    explicit Database(const std::string& connection_string);
    ~Database() = default;

    pqxx::connection& connection();
    std::vector<ActiveToken> fetch_active_tokens();
};