#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>
#include "grpc_service.h"
#include "config.h"
#include "database.h"
#include <csignal>
#include <atomic>
#include <thread>

std::atomic<bool> running{ true };

int main(int argc, char* argv[]) {
    spdlog::info("Match API starting...");
    std::string config_path = "config.json";
    if (argc > 1) config_path = argv[1];
    Config config(config_path);
    Database db(config.db_connection_string());
    MatchServiceImpl service(db, config.api_key());

    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    spdlog::info("gRPC server listening on 0.0.0.0:50051");

    signal(SIGINT, [](int) { running = false; });
    signal(SIGTERM, [](int) { running = false; });
    while (running) std::this_thread::sleep_for(std::chrono::seconds(1));

    server->Shutdown();
    return 0;
}