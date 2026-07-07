#include "match_processor.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "database.h"

MatchProcessor::MatchProcessor(pqxx::connection& conn, const ApiClient& api, const std::string& base_url)
    : conn_(conn), api_(api), base_url_(base_url) {}

void MatchProcessor::process_token(const ActiveToken& token) {
    std::string url = base_url_ + "?token=" + token.token;
    spdlog::info("Fetching token id {}", token.id);

    try {
        std::string response = api_.get(url);
        spdlog::info("Token {} raw response (first 500 chars): {}", token.id, response.substr(0, 500));
        parse_and_store(token.id, response);
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to process token {}: {}", token.id, e.what());
    }
}

void MatchProcessor::parse_and_store(int token_id, const std::string& json_response) {
    auto json = nlohmann::json::parse(json_response);
    if (!json.contains("matches") || !json["matches"].is_array()) {
        spdlog::warn("Token {}: 'matches' field missing or not an array", token_id);
        return;
    }

    pqxx::work txn(conn_);
    for (const auto& match : json["matches"]) {
        std::string mid = match.value("mid", "");
        std::string map_name = match.value("map_name", "unknown");
        bool aim_assist = match.value("aim_assist_allowed", false);
        int64_t match_start = match.value("match_start", 0);

        pqxx::result res = txn.exec_params(
            "INSERT INTO matches (token_id, mid, map_name, aim_assist, match_start) "
            "VALUES ($1, $2, $3, $4, to_timestamp($5)) "
            "ON CONFLICT (mid) DO UPDATE SET token_id = EXCLUDED.token_id "
            "RETURNING id",
            token_id, mid, map_name, aim_assist, match_start
        );
        int match_db_id = res[0][0].as<int>();

        if (match.contains("player_results") && match["player_results"].is_array()) {
            for (const auto& p : match["player_results"]) {
                txn.exec_params(
                    "INSERT INTO match_players (match_id, nid_hash, player_name, team_name, team_num, team_placement, "
                    "character_name, hardware, kills, assists, knockdowns, damage_dealt, headshots, shots, hits, survival_time, "
                    "respawns_given, revives_given) "
                    "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18)",
                    match_db_id,
                    p.value("nidHash", ""),
                    p.value("playerName", ""),
                    p.value("teamName", ""),
                    p.value("teamNum", 0),
                    p.value("teamPlacement", 0),
                    p.value("characterName", ""),
                    p.value("hardware", ""),
                    p.value("kills", 0),
                    p.value("assists", 0),
                    p.value("knockdowns", 0),
                    p.value("damageDealt", 0),
                    p.value("headshots", 0),
                    p.value("shots", 0),
                    p.value("hits", 0),
                    p.value("survivalTime", 0),
                    p.value("respawnsGiven", 0),
                    p.value("revivesGiven", 0)
                );
            }
        }
    }
    txn.commit();
    spdlog::info("Stored match data for token {}", token_id);
}