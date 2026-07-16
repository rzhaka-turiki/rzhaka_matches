#pragma once
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>

#include "common/database_base.h"
#include "game_modules.h"

class Database : public DatabaseBase {
public:
    using DatabaseBase::DatabaseBase;
    std::unordered_set<std::string> get_mids();
};

// operator overloading
Database& operator<<(Database& db, Match& s_match_);
Database& operator<<(Database& db, const Player& s_player_);