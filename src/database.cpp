#include "database.h"

#include <stdexcept>

std::unordered_set<std::string> Database::get_mids() {
    std::unordered_set<std::string> mids;
    pqxx::work txn(conn_);
    auto rows = txn.exec("SELECT mid FROM matches;");
    for (const auto& row : rows) {
        mids.insert(row[0].as<std::string>());
    }
    txn.commit();
    return mids;
}

Database& operator<<(Database& db, Match& s_match) {
    pqxx::work txn(db.connection());
    pqxx::result res = txn.exec_params(
        "INSERT INTO matches (token_id, mid, map_name, aim_assist, match_start) "
        "VALUES ($1, $2, $3, $4, to_timestamp($5)) "
        "ON CONFLICT (mid) DO UPDATE SET token_id = EXCLUDED.token_id "
        "RETURNING id",
        s_match.getToken(), s_match.getMid(), s_match.getMap(), s_match.getAimAssist(), s_match.getMatchStart());
    s_match.setDBid(res[0][0].as<int>());
    for (auto& p : s_match.getPlayers()) {
        p.setDBid(s_match.getDBid());
        p.insert_into(txn);
    }
    txn.commit();
    return db;
}