#pragma once
#include <string>

class ApiClient {
public:
    std::string get(const std::string& url) const;
};