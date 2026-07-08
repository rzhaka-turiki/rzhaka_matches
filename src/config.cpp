#include "config.h"
#include <fstream>
#include <stdexcept>

Config::Config(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + filepath);
    file >> data_;
}

std::string Config::db_connection_string() const {
    return data_.at("db_connection").get<std::string>();
}

std::string Config::api_base_url() const {
    return data_.at("api_base_url").get<std::string>();
}

int Config::fetch_interval_seconds() const {
    return data_.value("fetch_interval_seconds", 5);
}

int fetch_interval_seconds() const {
    return data_.value("token_cache_seconds", 60);
}