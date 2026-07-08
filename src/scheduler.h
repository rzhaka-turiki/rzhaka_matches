#pragma once
#include "database.h"
#include "token_cache.h"
#include "match_processor.h"
#include <atomic>
#include <chrono>

class Database;
class MatchProcessor;

class Scheduler {
private:
    Database& db_;
    MatchProcessor& processor_;
    TokenCache token_cache_;
    int interval_seconds_;
    std::atomic<bool> running_{ true };
public:
    Scheduler(Database& db, TokenCache& token_cache, MatchProcessor& processor, int interval_seconds);

    void run();   // блокирующий цикл, пока running == true
    void stop();  // запрос на остановку
};