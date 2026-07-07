FROM ubuntu:22.04 AS builder
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    libpqxx-dev \
    libspdlog-dev \
    && rm -rf /var/lib/apt/lists/*