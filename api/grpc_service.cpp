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

grpc::Status MatchServiceImpl::ListMatches(grpc::ServerContext* ctx,
    const matchapi::ListMatchesRequest* req,
    matchapi::ListMatchesResponse* resp) {
    if (!check_auth(ctx))
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid API key");

    std::string sql = "SELECT id, mid, map_name, match_start, "
        "(SELECT COUNT(*) FROM match_players WHERE match_id = matches.id) "
        "FROM matches WHERE 1=1";

    if (!req->start_date().empty()) sql += " AND match_start >= '" + req->start_date() + "'::timestamptz";
    if (!req->end_date().empty())   sql += " AND match_start <= '" + req->end_date() + "'::timestamptz";
    if (!req->map_name().empty())   sql += " AND map_name = '" + req->map_name() + "'";
    if (!req->player_hash().empty())
        sql += " AND id IN (SELECT match_id FROM match_players WHERE nid_hash = '" + req->player_hash() + "')";

    int page_size = req->page_size() ? req->page_size() : 50;
    int offset = 0;
    if (!req->page_token().empty()) offset = std::stoi(req->page_token());
    sql += " ORDER BY id DESC LIMIT " + std::to_string(page_size) + " OFFSET " + std::to_string(offset);

    try {
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
    }
    catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
    return grpc::Status::OK;
}