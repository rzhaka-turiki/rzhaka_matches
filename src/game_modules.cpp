#include "game_modules.h"

void Player::init(nlohmann::json s_json_) {
    nid_hash_ = s_json_.value("nidHash", "");
    player_name_ = s_json_.value("playerName", "");
    team_name_ = s_json_.value("teamName", "");
    team_num_ = s_json_.value("teamNum", 0);
    team_placement_ = s_json_.value("teamPlacement", 0);
    character_name_ = s_json_.value("characterName", "");
    hardware_ = s_json_.value("hradware", "");
    kills_ = s_json_.value("kills", 0);
    assists_ = s_json_.value("assists", 0);
    knockdowns_ = s_json_.value("knockdowns", 0);
    damage_dealt_ = s_json_.value("damageDealt", 0);
    headshots_ = s_json_.value("headshots", 0);
    shots_ = s_json_.value("shots", 0);
    hits_ = s_json_.value("hits", 0);
    survival_time_ = s_json_.value("survivalTime", 0);
    respawns_given_ = s_json_.value("respawnsGiven", 0);
    revives_given_ = s_json_.value("revivesGiven", 0);
}

void Player::insert_into(pqxx::work& txn) const {
    txn.exec_params(
        "INSERT INTO match_players (match_id, nid_hash, player_name, team_name, team_num, "
        "team_placement, "
        "character_name, hardware, kills, assists, knockdowns, damage_dealt, headshots, shots, hits, "
        "survival_time, "
        "respawns_given, revives_given) "
        "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18) "
        "ON CONFLICT (match_id, nid_hash) DO NOTHING",
        db_id_, nid_hash_, player_name_, team_name_, team_num_, team_placement_, character_name_, hardware_, kills_,
        assists_, knockdowns_, damage_dealt_, headshots_, shots_, hits_, survival_time_, respawns_given_,
        revives_given_);
    txn.commit();
}

void Match::init(nlohmann::json s_json_, const int s_token_id_ = 0) {
    json_ = s_json_;
    mid_ = json_.value("mid", "");
    token_id_ = s_token_id_;
    map_name_ = json_.value("map_name", "unknown");
    aim_assist_ = json_.value("aim_assist_allowed", false);
    match_start_ = json_.value("match_start", 0);
    if (json_.contains("player_results") && json_["player_results"].is_array()) {
        for (const auto& p_ : json_["player_results"]) {
            Player player;
            player.init(p_);
            players_.push_back(player);
        }
    }
}