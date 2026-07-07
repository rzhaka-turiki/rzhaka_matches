#pragma once
#include <atomic>
#include <chrono>

class Database;
class MatchProcessor;

class Scheduler {
public:
    Scheduler(Database& db, MatchProcessor& processor, int interval_seconds);

    void run();   // блокирующий цикл, пока running == true
    void stop();  // запрос на остановку

private:
    Database& db_;
    MatchProcessor& processor_;
    int interval_seconds_;
    std::atomic<bool> running_{ true };
};