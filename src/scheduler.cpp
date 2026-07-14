#include "scheduler.h"

#include <spdlog/spdlog.h>

Scheduler::Scheduler(Database& db, TokenCache& token_cache, MatchProcessor& processor, int interval_seconds)
    : db_(db), token_cache_(token_cache), processor_(processor), interval_seconds_(interval_seconds) {}

void Scheduler::run() {
    spdlog::info("Scheduler started, interval {}s", interval_seconds_);
    while (running_) {
        try {
            auto tokens = token_cache_.get_tokens();
            for (size_t i = 0; i < tokens.size() && running_; ++i) {
                try {
                    processor_.process_token(tokens[i]);
                } catch (const std::exception& e) {
                    spdlog::error("Token {} failed: {}", tokens[i].id, e.what());
                }

                if (i < tokens.size() - 1) {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
        } catch (const std::exception& e) {
            spdlog::error("Main loop error: {}", e.what());
        }

        for (int i = 0; i < interval_seconds_ && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    spdlog::info("Scheduler stopped");
}

void Scheduler::stop() { running_ = false; }