#pragma once
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>

#include "api_client.h"

struct ActiveToken;

class MatchProcessor {
private:
    pqxx::connection& conn_;
    const ApiClient& api_;
    std::string base_url_;

    void parse_and_store(int token_id, const std::string& json_response);
    // mids hash
    std::unordered_set<std::string> stored_mids_;
    std::mutex mids_mutex_;

public:
    MatchProcessor(pqxx::connection& conn, const ApiClient& api, const std::string& base_url);

    void process_token(const ActiveToken& token);
};