#include "grpc_service.h"
#include <spdlog/spdlog.h>
#include <sstream>

MatchServiceImpl::MatchServiceImpl(Database& db, const std::string& api_key)
    : db_(db), api_key_(api_key) {}

bool MatchServiceImpl::check_auth(grpc::ServerContext* ctx) {
    auto metadata = ctx->client_metadata();
    auto it = metadata.find("authorization");
    if (it == metadata.end()) return false;
    std::string val(it->second.data(), it->second.size());
    if (val.rfind("Bearer ", 0) != 0) return false;
    return val.substr(7) == api_key_;
}

grpc::Status MatchServiceImpl::AddToken(grpc::ServerContext* ctx,
    const matchapi::AddTokenRequest* req,
    matchapi::AddTokenResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        pqxx::work txn(db_.connection());
        auto res = txn.exec_params(
            "INSERT INTO tokens (activation, expiration, stats_token, admin_token, player_token) "
            "VALUES ($1::timestamptz, $2::timestamptz, $3, $4, $5) RETURNING id",
            req->activation(), req->expiration(), req->stats_token(),
            req->admin_token(), req->player_token());
        int id = res[0][0].as<int>();
        txn.commit();
        resp->set_token_id(id);
        resp->set_message("Token added");
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        spdlog::error("AddToken: {}", e.what());
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status MatchServiceImpl::ListTokens(grpc::ServerContext* ctx,
    const matchapi::ListTokensRequest* req,
    matchapi::ListTokensResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        std::string sql = "SELECT id, activation, expiration, "
            "(NOW() BETWEEN activation AND expiration) as active FROM tokens";
        if (req->only_active())
            sql += " WHERE NOW() BETWEEN activation AND expiration";
        sql += " ORDER BY id DESC";

        // Пагинация
        int page_size = req->page_size() > 0 ? req->page_size() : 50;
        int offset = 0;
        if (!req->page_token().empty()) offset = std::stoi(req->page_token());
        sql += " LIMIT " + std::to_string(page_size) + " OFFSET " + std::to_string(offset);

        pqxx::work txn(db_.connection());
        auto res = txn.exec(sql);
        for (const auto& row : res) {
            auto* t = resp->add_tokens();
            t->set_id(row[0].as<int>());
            t->set_activation(row[1].as<std::string>());
            t->set_expiration(row[2].as<std::string>());
            t->set_is_active(row[3].as<bool>());
        }
        if (res.size() == page_size)
            resp->set_next_page_token(std::to_string(offset + page_size));
        resp->set_total_count(0); // можно отдельный COUNT
        txn.commit();
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status MatchServiceImpl::ListMatches(grpc::ServerContext* ctx,
    const matchapi::ListMatchesRequest* req,
    matchapi::ListMatchesResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        std::string sql = "SELECT id, mid, map_name, match_start, "
            "(SELECT COUNT(*) FROM match_players WHERE match_id = matches.id) "
            "FROM matches WHERE 1=1";

        if (!req->start_date().empty())
            sql += " AND match_start >= '" + req->start_date() + "'::timestamptz";
        if (!req->end_date().empty())
            sql += " AND match_start <= '" + req->end_date() + "'::timestamptz";
        if (!req->map_name().empty())
            sql += " AND map_name = '" + req->map_name() + "'";
        if (!req->player_hash().empty())
            sql += " AND id IN (SELECT match_id FROM match_players WHERE nid_hash = '" + req->player_hash() + "')";

        int page_size = req->page_size() > 0 ? req->page_size() : 50;
        int offset = 0;
        if (!req->page_token().empty()) offset = std::stoi(req->page_token());
        sql += " ORDER BY id DESC LIMIT " + std::to_string(page_size) + " OFFSET " + std::to_string(offset);

        pqxx::work txn(db_.connection());
        auto res = txn.exec(sql);
        for (const auto& row : res) {
            auto* m = resp->add_matches();
            m->set_id(row[0].as<int>());
            m->set_mid(row[1].as<std::string>());
            m->set_map_name(row[2].as<std::string>());
            m->set_match_start(row[3].as<std::string>());
            m->set_player_count(row[4].as<int>());
        }
        if (res.size() == page_size)
            resp->set_next_page_token(std::to_string(offset + page_size));
        resp->set_total_count(0);
        txn.commit();
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status MatchServiceImpl::GetMatchStats(grpc::ServerContext* ctx,
    const matchapi::MatchStatsRequest* req,
    matchapi::MatchStatsResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        std::string sql = "SELECT COUNT(*)::int, "
            "COALESCE(AVG(ps.kills),0), COALESCE(AVG(ps.damage_dealt),0), "
            "COALESCE(AVG(ps.survival_time),0), "
            "(SELECT COUNT(*) FROM match_players WHERE match_id IN "
            "(SELECT id FROM matches WHERE 1=1";
        // Собираем условия для фильтров основного запроса
        std::string cond;
        if (!req->start_date().empty())
            cond += " AND match_start >= '" + req->start_date() + "'::timestamptz";
        if (!req->end_date().empty())
            cond += " AND match_start <= '" + req->end_date() + "'::timestamptz";
        if (!req->map_name().empty())
            cond += " AND map_name = '" + req->map_name() + "'";
        if (!req->player_hash().empty())
            cond += " AND id IN (SELECT match_id FROM match_players WHERE nid_hash = '" + req->player_hash() + "')";

        sql += cond + ")) as total_players "
            "FROM matches m "
            "JOIN match_players ps ON ps.match_id = m.id WHERE 1=1" + cond;

        pqxx::work txn(db_.connection());
        auto res = txn.exec(sql);
        if (!res.empty()) {
            resp->set_total_matches(res[0][0].as<int>());
            resp->set_total_players(res[0][4].as<int>()); // total_players
            resp->set_avg_kills(res[0][1].as<double>());
            resp->set_avg_damage(res[0][2].as<double>());
            resp->set_avg_survival_time(res[0][3].as<double>());
        }
        txn.commit();
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status MatchServiceImpl::GetMatchDetail(grpc::ServerContext* ctx,
    const matchapi::MatchDetailRequest* req,
    matchapi::MatchDetailResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        pqxx::work txn(db_.connection());
        auto match_res = txn.exec_params("SELECT id, mid, map_name, aim_assist, match_start FROM matches WHERE id=$1",
            req->match_id());
        if (match_res.empty())
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Match not found");
        auto& m = match_res[0];
        resp->set_match_id(m[0].as<int>());
        resp->set_mid(m[1].as<std::string>());
        resp->set_map_name(m[2].as<std::string>());
        resp->set_aim_assist_allowed(m[3].as<bool>());
        resp->set_match_start(m[4].as<std::string>());

        auto players = txn.exec_params("SELECT player_name, character_name, kills, assists, knockdowns, "
            "damage_dealt, survival_time, team_name, team_placement, nid_hash "
            "FROM match_players WHERE match_id=$1 ORDER BY id", req->match_id());
        for (const auto& p : players) {
            auto* player = resp->add_players();
            player->set_player_name(p[0].as<std::string>());
            player->set_character_name(p[1].as<std::string>());
            player->set_kills(p[2].as<int>());
            player->set_assists(p[3].as<int>());
            player->set_knockdowns(p[4].as<int>());
            player->set_damage_dealt(p[5].as<int>());
            player->set_survival_time(p[6].as<int>());
            player->set_team_name(p[7].as<std::string>());
            player->set_team_placement(p[8].as<int>());
            player->set_nid_hash(p[9].as<std::string>());
        }
        txn.commit();
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status MatchServiceImpl::GetPlayerStats(grpc::ServerContext* ctx,
    const matchapi::PlayerStatsRequest* req,
    matchapi::PlayerStatsResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");
    try {
        std::string cond = " WHERE nid_hash = '" + req->player_hash() + "'";
        if (!req->start_date().empty())
            cond += " AND match_start >= '" + req->start_date() + "'::timestamptz";
        if (!req->end_date().empty())
            cond += " AND match_start <= '" + req->end_date() + "'::timestamptz";

        pqxx::work txn(db_.connection());
        auto res = txn.exec_params(
            "SELECT COUNT(DISTINCT m.id)::int, COALESCE(SUM(mp.kills),0), "
            "COALESCE(SUM(mp.damage_dealt),0), COALESCE(AVG(mp.damage_dealt),0), "
            "COALESCE(AVG(mp.survival_time),0), "
            "SUM(CASE WHEN mp.team_placement=1 THEN 1 ELSE 0 END)::int "
            "FROM match_players mp JOIN matches m ON mp.match_id=m.id" + cond);
        if (!res.empty()) {
            resp->set_player_hash(req->player_hash());
            resp->set_matches_played(res[0][0].as<int>());
            resp->set_total_kills(res[0][1].as<int>());
            resp->set_total_damage(res[0][2].as<int>());
            resp->set_avg_damage(res[0][3].as<double>());
            resp->set_avg_survival_time(res[0][4].as<double>());
            resp->set_total_wins(res[0][5].as<int>());
        }

        // Топ персонажей
        auto chars = txn.exec_params(
            "SELECT character_name, COUNT(*) as matches, SUM(kills) as kills, COUNT(*) as picks "
            "FROM match_players WHERE nid_hash=$1 GROUP BY character_name ORDER BY matches DESC LIMIT 5",
            req->player_hash());
        for (const auto& c : chars) {
            auto* cs = resp->add_top_characters();
            cs->set_character_name(c[0].as<std::string>());
            cs->set_matches(c[1].as<int>());
            cs->set_kills(c[2].as<int>());
            cs->set_picks(c[3].as<int>());   // picks = количество матчей с этим персонажем
        }
        txn.commit();
        return grpc::Status::OK;
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}