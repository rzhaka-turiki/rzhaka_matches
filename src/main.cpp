#include <spdlog/spdlog.h>

#include <csignal>
#include <iostream>

#include "api_client.h"
#include "config.h"
#include "database.h"
#include "match_processor.h"
#include "scheduler.h"
#include "token_cache.h"

std::atomic<bool> global_running{true};

void signal_handler(int) { global_running = false; }

int main(int argc, char* argv[]) {
    spdlog::info("Private Match Handler starting...");

    // cfg read
    std::string config_path = "config.json";
    if (argc > 1) config_path = argv[1];
    Config config(config_path);

    // init
    Database db(config.db_connection_string());
    TokenCache token_cache(db, config.fetch_cache_seconds());
    ApiClient api;
    MatchProcessor processor(db, api, config.api_base_url());
    processor.init_mids(db.get_mids());
    Scheduler scheduler(db, token_cache, processor, config.fetch_interval_seconds());

    // signal handler
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // scheduler run
    scheduler.run();

    spdlog::info("Shutting down.");
    return 0;
}