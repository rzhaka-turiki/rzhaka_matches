#pragma once
#include <grpcpp/grpcpp.h>
#include "match_service.grpc.pb.h"
#include "database.h"
#include <string>

class MatchServiceImpl final : public matchapi::MatchService::Service {
private:
    Database& db_;
    std::string api_key_;
    bool check_auth(grpc::ServerContext* ctx);
public:
    MatchServiceImpl(Database& db, const std::string& api_key);

    grpc::Status AddToken(grpc::ServerContext* ctx, const matchapi::AddTokenRequest* req,
        matchapi::AddTokenResponse* resp) override;
    grpc::Status ListTokens(grpc::ServerContext* ctx, const matchapi::ListTokensRequest* req,
        matchapi::ListTokensResponse* resp) override;
    grpc::Status ListMatches(grpc::ServerContext* ctx, const matchapi::ListMatchesRequest* req,
        matchapi::ListMatchesResponse* resp) override;
    grpc::Status GetMatchStats(grpc::ServerContext* ctx, const matchapi::MatchStatsRequest* req,
        matchapi::MatchStatsResponse* resp) override;
    grpc::Status GetMatchDetail(grpc::ServerContext* ctx, const matchapi::MatchDetailRequest* req,
        matchapi::MatchDetailResponse* resp) override;
    grpc::Status GetPlayerStats(grpc::ServerContext* ctx, const matchapi::PlayerStatsRequest* req,
        matchapi::PlayerStatsResponse* resp) override;
};