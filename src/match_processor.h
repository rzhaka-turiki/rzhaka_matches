#pragma once
#include <iostream>
#include <mutex>
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>

#include "api_client.h"
#include "game_modules.h"
#include "database.h"

struct ActiveToken;

class MatchProcessor {
private:
    Database& db_;
    const ApiClient& api_;
    std::string base_url_;

    void parse_and_store(int token_id, const std::string& json_response);
    // mid_hashes
    std::unordered_set<std::string> mids_cache_;
    std::mutex mids_mutex_;

public:
    MatchProcessor(Database& db, const ApiClient& api, const std::string& base_url);

    Database getDB() { return db_; }
    void process_token(const ActiveToken& token);
    void init_mids(std::unordered_set<std::string> s_mids_) { mids_cache_ = s_mids_;}
};