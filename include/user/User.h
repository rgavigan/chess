/**
 * @file User.h
 * @brief Declaration of the User class.
 *
 * This file contains the declaration of the User class, which represents a user in the system.
 * It includes methods to access and manipulate user data such as username and game statistics.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#ifndef USER_H
#define USER_H

#include <vector>
#include <string>
#include <chrono>
#include "UserManager.h"

/**
 * @class User
 * @brief Represents a user in the system.
 */
class User {
public:
    /** Constructs a User object with a given username. */
    User(const std::string& username);

    /** Retrieves the username of the user. */
    const std::string& getUsername() const;

    /** Retrieves the number of wins of the user. */
    int getWins() const;

    /** Retrieves the number of losses of the user. */
    int getLosses() const;

    /** Retrieves the number of draws of the user. */
    int getDraws() const;

    /** Increments the number of wins of the user. */
    void incrementWins();

    /** Increments the number of losses of the user. */
    void incrementLosses();

    /** Increments the number of draws of the user. */
    void incrementDraws();

    /** Initializes the stats of the user. */
    void initializeStats(int wins, int losses, int draws);

    /** Sets the username of the user. */
    void setUsername(const std::string& username);

    /** Resets the user instance to default values. */
    void resetUser();
    
    /** Adds a game to the user's game list. */
    void addGame(int id);

    /** Gets the list of the user's saved games. */
    std::vector<int> getGames();

    /** Set the timer of the User for player instantiation */
    void setTimer(std::chrono::minutes time);

    /** Get the timer for the user */
    std::chrono::minutes getTimer();

private:
    std::string username; ///< The username of the user.
    int wins = 0; ///< The number of wins of the user.
    int losses = 0; ///< The number of losses of the user.
    int draws = 0; ///< The number of draws of the user.
    std::vector<int> games; ///< List of user's saved games.
    std::chrono::minutes timer = std::chrono::minutes(10); ///< The user's selected timer.
};

#endif // USER_H
