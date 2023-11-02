/**
 * @file User.cpp
 * @brief Implementation of the User class.
 *
 * This file contains the implementation of the methods declared in User.h.
 * The User class holds persistent data related to a user, such as username, hashed password,
 * and game statistics (wins, losses, draws).
 * It provides methods to interact with and manipulate this data, allowing for user verification and stats update.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#include <string>
#include <iostream>
#include <crypt.h>
#include "User.h"

/**
 * @brief Constructs a User object with a given username.
 * @param username The username of the user.
 */
User::User(const std::string& username) : username(username) {

}

/**
 * @brief Retrieves the username of the user.
 * @return The username of the user.
 */
const std::string& User::getUsername() const {
    return username;
}

/**
 * @brief Retrieves the number of wins of the user.
 * @return The number of wins of the user.
 */
int User::getWins() const {
    return wins;
}

/**
 * @brief Retrieves the number of losses of the user.
 * @return The number of losses of the user.
 */
int User::getLosses() const {
    return losses;
}

/**
 * @brief Retrieves the number of draws of the user.
 * @return The number of draws of the user.
 */
int User::getDraws() const {
    return draws;
}

/**
 * @brief Increments the number of wins of the user.
 */
void User::incrementWins() {
    ++wins;
}

/**
 * @brief Increments the number of losses of the user.
 */
void User::incrementLosses() {
    ++losses;
}

/**
 * @brief Increments the number of draws of the user.
 */
void User::incrementDraws() {
    ++draws;
}

/**
 * @brief Initializes the stats of the user.
 * @param wins The number of wins to set.
 * @param losses The number of losses to set.
 * @param draws The number of draws to set.
 */
void User::initializeStats(int wins, int losses, int draws) {
    this->wins = wins;
    this->losses = losses;
    this->draws = draws;
}

/**
 * @brief Set the username of the user.
 */
void User::setUsername(const std::string& username) {
    this->username = username;
}

/**
 * @brief Resets the user instance.
 */
void User::resetUser() {
    std::string username = "";
    this->setUsername(username);
    this->initializeStats(0, 0, 0);
}

/**
 * @brief Adds a game to the user's game list.
 * @param id Game ID to add.
*/
void User::addGame(int id) {
    games.push_back(id);
}

/**
 * @brief Gets the list of the user's saved games.
 * @return List of User's saved game IDs.
*/
std::vector<int> User::getGames() {
    return games;
}

/**
 * @brief Sets the timer of the User for player instantiation
 * @param time time to set as the user's timer
 */
void User::setTimer(std::chrono::minutes time) {
    this->timer = time;
}

std::chrono::minutes User::getTimer() {
    return timer;
}
