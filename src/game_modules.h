#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Player {
private:
    int db_id_;
    std::string nid_hash_;
    int kills_;
    std::string character_name_;
    std::string team_name_;
    int team_num_;
    int team_placement_;
    std::string player_name_;
    std::string hardware_;
    int assists_;
    int knockdowns_;
    int damage_dealt_;
    int headshots_;
    int shots_;
    int hits_;
    int survival_time_;
    int respawns_given_;
    int revives_given_;
    nlohmann::json json_;

public:
    // get
    int getDBid() const { return db_id_; }
    int getKills() const { return kills_; }
    int getTeamNum() const { return team_num_; }
    int getTeamPlacement() const { return team_placement_; }
    int getAssists() const { return assists_; }
    int getKnockdowns() const { return knockdowns_; }
    int getDamage() const { return damage_dealt_; }
    int getHeadshots() const { return headshots_; }
    int getShots() const { return shots_; }
    int getHits() const { return hits_; }
    int getSurvivalTime() const { return survival_time_; }
    int getRespawnsGiven() const { return respawns_given_; }
    int getRevivesGiven() const { return revives_given_; }
    std::string getNidHash() const { return nid_hash_; }
    std::string getPlayerName() const { return player_name_; }
    std::string getTeamName() const { return team_name_; }
    std::string getHardware() const { return hardware_; }
    std::string getCharacterName() const { return character_name_; }
    // set
    void init(nlohmann::json s_json_);
    void setDBid(int s_db_id_) { db_id_ = s_db_id_; }
};

class Match {
private:
    std::string mid_;
    int token_id_;
    std::string map_name_;
    bool aim_assist_;
    int64_t match_start_;
    std::vector<Player> players_;
    int db_id_;

    nlohmann::json json_;

public:
    // get
    std::string getMid() const { return mid_; }
    int getToken() const { return token_id_; }
    std::string getMap() const { return map_name_; }
    bool getAimAssist() const { return aim_assist_; }
    int64_t getMatchStart() const { return match_start_; }
    std::vector<Player> getPlayers() const { return players_; }
    int getDBid() const { return db_id_; }
    nlohmann::json getJson() const { return json_; }

    // set
    void init(nlohmann::json s_json_, const int s_token_id_);
    void setDBid(const int s_db_id_) { db_id_ = s_db_id_; }
};