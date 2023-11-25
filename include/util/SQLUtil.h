/**
 * @file SQLUtil.h
 * @brief This is the header file for the SQLUtil class. This class contains SQL-related utility functions.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#ifndef SQLUTIL_H
#define SQLUTIL_H

#include <string>
#include <iostream>
#include <sqlite3.h>

class SQLUtil {
public:
    static bool columnExists(sqlite3* db, const std::string& tableName, const std::string& columnName);
    static void addColumnIfNotExists(sqlite3* db, const std::string& tableName, const std::string& columnName, const std::string& columnType);
};

#endif // SQLUTIL_H