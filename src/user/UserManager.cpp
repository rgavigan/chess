/**
 * @file UserManager.cpp
 * @brief Implementation of the UserManager class.
 *
 * This file contains the implementation of the methods declared in UserManager.h.
 * The UserManager class is responsible for managing User data, including creating users
 * and authenticating User logins, by interacting with the SQLite database.
 * It provides methods to interact with and manipulate User data, allowing for User
 * creation, User authentication, and password hashing.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#include <iostream>
#include <sqlite3.h>
#include <functional>
#include <string>
#include <stdexcept>
#include "UserManager.h"

/**
 * @brief Constructs a UserManager object and initializes the SQLite database.
 * @param dbPath The path to the SQLite database.
 */
UserManager::UserManager(const std::string& dbPath) {
    // Open the SQLite database 
    if (sqlite3_open(dbPath.c_str(), &db)) {
        std::string errorMsg = "Can't open database: " + std::string(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw std::runtime_error(errorMsg);
    }

    // Create the USERS table if it does not exist 
    char* errMsg;
    std::string sql = "CREATE TABLE IF NOT EXISTS USERS("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "USERNAME TEXT NOT NULL, "
                      "PASSWORD TEXT NOT NULL, "
                      "WINS INTEGER DEFAULT 0, "
                      "LOSSES INTEGER DEFAULT 0, "
                      "DRAWS INTEGER DEFAULT 0);";

    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::string errorMsg = "SQL Error: " + std::string(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw std::runtime_error(errorMsg);
    }
}

/**
 * @brief Destructor for the UserManager class. Closes the SQLite database.
 */
UserManager::~UserManager() {
    sqlite3_close(db);
}

/**
 * @brief Creates a new User with the given username and password.
 * @param username The username of the new User.
 * @param password The password of the new User.
 * @return true if the User was successfully created, false otherwise.
 */
bool UserManager::createUser(const std::string& username, const std::string& password) {
    // Check for duplicates 
    if (checkForDuplicates(username)) {
        std::cerr << "Username already exists!" << std::endl;
        return false;
    }

    // If username does not exist, create a new User 
    std::string hashedPassword = std::to_string(hashPassword(password));
    std::string sqlInsert = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
    }
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    return false;
}

/**
 * @brief Authenticates a User with the given username and password.
 * @param username The username of the User.
 * @param password The password of the User.
 * @return true if the User was successfully authenticated, false otherwise.
 */
bool UserManager::authenticateUser(const std::string& username, const std::string& password) {
    // Prepare the SQL statement 
    std::string sql = "SELECT PASSWORD FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind the username to the SQL statement 
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement and compare the hashed passwords 
    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        const unsigned char* hashedPassword = sqlite3_column_text(stmt, 0);
        std::string enteredHashedPassword = std::to_string(hashPassword(password));

        bool passwordIsSame = enteredHashedPassword == reinterpret_cast<const char*>(hashedPassword);
        sqlite3_finalize(stmt);
        return passwordIsSame;
    }

    sqlite3_finalize(stmt);
    return false;
}

/**
 * @brief Hashes a password using std::hash.
 * @param password The password to be hashed.
 * @return The hashed password as a size_t.
 */
std::size_t UserManager::hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    return hasher(password);
}

/**
 * @brief Retrieves the statistics of a User.
 * @param username The username of the User.
 * @param wins A reference to the number of wins of the User.
 * @param losses A reference to the number of losses of the User.
 * @param draws A reference to the number of draws of the User.
 * @return true if the User was found, false otherwise.
 */
bool UserManager::getUserStats(const std::string& username, int& wins, int& losses, int& draws) const {

    std::string sql = "SELECT WINS, LOSSES, DRAWS FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        wins = sqlite3_column_int(stmt, 0);
        losses = sqlite3_column_int(stmt, 1);
        draws = sqlite3_column_int(stmt, 2);
    }

    sqlite3_finalize(stmt);
    return step == SQLITE_ROW;
}

/**
 * @brief Updates the statistics of a User.
 * @param username The username of the User.
 * @param wins The number of wins of the User.
 * @param losses The number of losses of the User.
 * @param draws The number of draws of the User.
 * @return true if the User was found, false otherwise.
 */
bool UserManager::updateUserStats(const std::string& username, int wins, int losses, int draws) {
    if (!checkForDuplicates(username)) {
        std::cerr << "Username does not exist!" << std::endl;
        return false;
    }

    std::string sql = "UPDATE USERS SET WINS = ?, LOSSES = ?, DRAWS = ? WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, wins);
    sqlite3_bind_int(stmt, 2, losses);
    sqlite3_bind_int(stmt, 3, draws);
    sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_STATIC);

    int step = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return step == SQLITE_DONE;
}

/**
 * @brief Checks if a User with the given username already exists.
 * @param username The username of the User.
 * @return true if the User was found, false otherwise.
 */
bool UserManager::checkForDuplicates(const std::string& username) const {
    // Check if username already exists 
    std::string sqlSelect = "SELECT COUNT(*) FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, sqlSelect.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    return count > 0;
}
