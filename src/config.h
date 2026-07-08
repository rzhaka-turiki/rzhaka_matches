#pragma once
#include <string>
#include <nlohmann/json.hpp>

class Config {
private:
    nlohmann::json data_;
    int token_cache_sec
public:
    Config(const std::string& filepath);

    std::string db_connection_string() const;
    std::string api_base_url() const;
    int fetch_interval_seconds() const;
};