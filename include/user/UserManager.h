/**
 * @file UserManager.h
 * @brief Declaration of the UserManager class.
 *
 * This class is responsible for managing user accounts,
 * including creating new accounts, verifying logins, and managing user data.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <unordered_map>
#include <string>
#include <sqlite3.h>
#include "SQLUtil.h"

class User;

/**
 * @class UserManager
 * @brief Manages user accounts and related operations.
 *
 * This class provides functionality for managing user accounts, including
 * user creation, login verification, and user data management.
 */
class UserManager {
public:
    /** Constructor: Initializes a UserManager object. */
    UserManager(const std::string& dbPath);

    /**
     * @brief Destructor for the UserManager class.
     *
     * Closes the SQLite database.
     */
    ~UserManager();

    /** Create a new user account. */
    bool createUser(const std::string& username, const std::string& password);

    /** Verify user login. */
    bool authenticateUser(const std::string& username, const std::string& password);

    /** Hash user passwords. */
    std::size_t hashPassword(const std::string& password);

    /** Get a user's statistics. */
    bool getUserStats(const std::string& username, int& wins, int& losses, int& draws, double& elo) const;

    /** Update a user's statistics. */
    bool updateUserStats(const std::string& username, int wins, int losses, int draws, double elo);

    /** Check for duplicate usernames. */
    bool checkForDuplicates(const std::string& username) const;

private:
    sqlite3* db;
};

#endif // USER_MANAGER_H
