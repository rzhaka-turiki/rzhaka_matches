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

void Match::init(nlohmann::json s_json_, const int s_token_id_) { 
    json_ = s_json_;
    mid_ = json_.value("mid", "");
    token_id_ = token_id_;
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