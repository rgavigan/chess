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
#include <cmath>
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

    /** Gets the user's Elo rating. */
    double getElo();

    /** Gets the user's Elo rating rounded to an integer. */
    int getRoundedElo();

    /** Updates the user's Elo rating. */
    void updateElo(double eloChange);

    /** Initializes the stats of the user. */
    void initializeStats(int wins, int losses, int draws, double elo);

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
    double elo = 1000.0; ///< The user's Elo rating.
    std::vector<int> games; ///< List of user's saved games.
    std::chrono::minutes timer = std::chrono::minutes(10); ///< The user's selected timer.
};

#endif // USER_H
