#pragma once

#include <boost/log/trivial.hpp>
#include <cstddef>
#include <sqlite3.h>
#include <string>

namespace orderbook
{
    inline void create_users_table(sqlite3* db)
    {
        constexpr auto sql = "CREATE TABLE IF NOT EXISTS USERS("
                             "ID INT PRIMARY KEY NOT NULL,"
                             "USERNAME TEXT UNIQUE NOT NULL)";
        char* error_message;
        const int rc = sqlite3_exec(db, sql, NULL, 0, &error_message);

        if (rc != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error)
                << "Error creating users table: " << error_message;
            sqlite3_free(error_message);
        }
    }

    inline int create_user(sqlite3* db, const std::string& username)
    {
        constexpr auto select_sql = "SELECT ID FROM USERS WHERE USERNAME = ?";
        sqlite3_stmt* select_stmt;
        int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);

        if (rc != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error)
                << "Error preparing select statement: " << sqlite3_errmsg(db);
            return 0;
        }

        sqlite3_bind_text(select_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(select_stmt) == SQLITE_ROW) {
            const int id = sqlite3_column_int(select_stmt, 0);
            sqlite3_finalize(select_stmt);
            return id;
        }
        sqlite3_finalize(select_stmt);

        constexpr auto insert_sql = "INSERT INTO USERS(USERNAME) VALUES(?)";
        sqlite3_stmt* insert_stmt;
        rc = sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, NULL);

        if (rc != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error)
                << "Error preparing insert statement: " << sqlite3_errmsg(db);
            return 0;
        }

        sqlite3_bind_text(insert_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        const int step = sqlite3_step(insert_stmt);
        if (step != SQLITE_DONE) {
            BOOST_LOG_TRIVIAL(error)
                << "Error executing insert statement: " << sqlite3_errmsg(db);
            sqlite3_finalize(insert_stmt);
            return 0;
        }

        sqlite3_finalize(insert_stmt);
        return static_cast<int>(sqlite3_last_insert_rowid(db));
    }
} // namespace orderbook