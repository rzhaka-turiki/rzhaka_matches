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

std::unordered_set<std::string> Database::get_mids() { 
    std::unordered_set<std::string> mids; 
    pqxx::work txn(conn_);
    auto rows = txn.exec("SELECT mid FROM matches;");
    for (const auto& row : rows) {
        mids.insert(row[0].as<std::string>());
    }
    txn.commit();
    return mids;
}

Database& operator<<(Database& db, const Player& s_player) {
    pqxx::work txn(db.connection());
    txn.exec_params(
        "INSERT INTO match_players (match_id, nid_hash, player_name, team_name, team_num, "
        "team_placement, "
        "character_name, hardware, kills, assists, knockdowns, damage_dealt, headshots, shots, hits, "
        "survival_time, "
        "respawns_given, revives_given) "
        "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18) "
        "ON CONFLICT (match_id, nid_hash) DO NOTHING",
        s_player.getDBid(), s_player.getNidHash(), s_player.getPlayerName(), s_player.getTeamName(), s_player.getTeamNum(), s_player.getTeamPlacement(),
        s_player.getCharacterName(), s_player.getHardware(), s_player.getKills(), s_player.getAssists(), s_player.getKnockdowns(), s_player.getDamage(),
        s_player.getHeadshots(), s_player.getShots(), s_player.getHits(), s_player.getSurvivalTime(), s_player.getRespawnsGiven(), s_player.getRevivesGiven());
    txn.commit();
}

Database& operator<<(Database& db, Match& s_match) { 
    pqxx::work txn(db.connection());
    pqxx::result res = txn.exec_params("INSERT INTO matches (token_id, mid, map_name, aim_assist, match_start) "
                "VALUES ($1, $2, $3, $4, to_timestamp($5)) "
                "ON CONFLICT (mid) DO UPDATE SET token_id = EXCLUDED.token_id "
        "RETURNING id",
        s_match.getToken(), s_match.getMid(), s_match.getMap(), s_match.getAimAssist(), s_match.getMatchStart());
    s_match.setDBid(res[0][0].as<int>());
    for (auto& p : s_match.getPlayers()) {
        p.setDBid(s_match.getDBid());
        db << p;
    }
    txn.commit();
}