#pragma once
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>

#include "game_modules.h"

struct ActiveToken {
    int id;
    std::string token;
};

class Database {
private:
    pqxx::connection conn_;

public:
    explicit Database(const std::string& connection_string);
    Database(Database& db);
    ~Database() = default;

    pqxx::connection& connection();
    std::vector<ActiveToken> fetch_active_tokens();
    std::unordered_set<std::string> get_mids();
};

// operator overloading
Database& operator<<(Database& db, Match& s_match_);
Database& operator<<(Database& db, const Player& s_player_);