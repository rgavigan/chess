/**
 * @file SQLUtil.cpp
 * @brief This is the implementation file for the SQLUtil class. This class contains SQL-related utility functions.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#include "SQLUtil.h"

/**
 * @brief Determines if a column exists in a given database.
 * @param db Database reference.
 * @param tableName Name of table to search.
 * @param columnName Name of column to search for.
 * @returns True if column exists, False otherwise.
*/
bool SQLUtil::columnExists(sqlite3* db, const std::string& tableName, const std::string& columnName) {
    bool exists = false;

    std::string query = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string currentColumnName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (currentColumnName == columnName) {
                exists = true;
                break;
            }
        }
    }

    sqlite3_finalize(stmt);
    return exists;
}

/**
 * @brief Adds a column to the database only if it doesn't already exist.
 * @param db Database reference.
 * @param tableName Name of table.
 * @param columnName Name of column to add.
 * @param columnType Column parameters.
*/
void SQLUtil::addColumnIfNotExists(sqlite3* db, const std::string& tableName, const std::string& columnName, const std::string& columnType) {
    if (!columnExists(db, tableName, columnName)) {
        std::string alterCmd = "ALTER TABLE " + tableName + " ADD COLUMN " + columnName + " " + columnType + ";";

        char* errMsg;
        if (sqlite3_exec(db, alterCmd.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL Error: " << std::string(errMsg) << std::endl;
            sqlite3_free(errMsg);
        }
    }
}
