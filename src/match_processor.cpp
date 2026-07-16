#include "match_processor.h"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

MatchProcessor::MatchProcessor(Database& db, const ApiClient& api, const std::string& base_url)
    : db_(db), api_(api), base_url_(base_url) {}

void MatchProcessor::process_token(const ActiveToken& token) {
    std::string url = base_url_ + "?token=" + token.token;
    spdlog::info("Fetching token id {}", token.id);

    try {
        std::string response = api_.get(url);
        parse_and_store(token.id, response);
    } catch (const std::exception& e) {
        spdlog::error("Failed to process token {}: {}", token.id, e.what());
    }
}

void MatchProcessor::parse_and_store(int token_id, const std::string& json_response) {
    auto json = nlohmann::json::parse(json_response);
    if (!json.contains("matches") || !json["matches"].is_array()) {
        spdlog::warn("Token {}: 'matches' field missing or not an array", token_id);
        return;
    }

    for (const auto& match : json["matches"]) {
        Match match_;
        match_.init(match, token_id);
        if (mids_cache_.find(match_.getMid()) != mids_cache_.end()) {
            continue;
        } else {
            db_ << match_;
            mids_cache_.insert(match_.getMid());
        }
    }
    spdlog::info("Stored match data for token {}", token_id);
}