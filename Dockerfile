FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    libcurl4 libpqxx-6.4 libspdlog1 ca-certificates && \
    rm -rf /var/lib/apt/lists/*
COPY --from=builder /app/build/fetcher /usr/local/bin/fetcher
CMD ["fetcher", "/etc/fetcher/config.json"]