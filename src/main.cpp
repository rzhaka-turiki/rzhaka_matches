#include <csignal>
#include <iostream>
#include "config.h"
#include "database.h"
#include "api_client.h"
#include "token_cache.h"
#include "match_processor.h"
#include "scheduler.h"
#include <spdlog/spdlog.h>

std::atomic<bool> global_running{ true };

void signal_handler(int) {
    global_running = false;
}

int main(int argc, char* argv[]) {
    spdlog::info("Private Match Fetcher starting...");

    // Чтение конфига
    std::string config_path = "config.json";
    if (argc > 1) config_path = argv[1];
    Config config(config_path);

    // Инициализация компонентов
    Database db(config.db_connection_string());
    TokenCache token_cache(db.connection(), config.fetch_cache_seconds());
    ApiClient api;
    MatchProcessor processor(db.connection(), api, config.api_base_url());
    Scheduler scheduler(db, token_cache, processor, config.fetch_interval_seconds());

    // Перехват сигналов
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Запуск планировщика (блокирующий)
    scheduler.run();

    spdlog::info("Shutting down.");
    return 0;
}