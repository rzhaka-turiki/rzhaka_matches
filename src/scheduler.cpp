#include "scheduler.h"
#include "database.h"
#include "match_processor.h"
#include <spdlog/spdlog.h>
#include <thread>

Scheduler::Scheduler(Database& db, MatchProcessor& processor, int interval_seconds)
    : db_(db), processor_(processor), interval_seconds_(interval_seconds) {}

void Scheduler::run() {
    spdlog::info("Scheduler started, interval {}s", interval_seconds_);
    while (running_) {
        try {
            auto tokens = db_.fetch_active_tokens();
            for (const auto& token : tokens) {
                try {
                    processor_.process_token(token);
                }
                catch (const std::exception& e) {
                    spdlog::error("Token {} failed: {}", token.id, e.what());
                }
            }
        }
        catch (const std::exception& e) {
            spdlog::error("Main loop error: {}", e.what());
        }

        // Спим с проверкой флага остановки каждую секунду
        for (int i = 0; i < interval_seconds_ && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    spdlog::info("Scheduler stopped");
}

void Scheduler::stop() {
    running_ = false;
}