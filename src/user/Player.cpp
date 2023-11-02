/**
 * @file Player.cpp
 * @brief Implementation of the Player class.
 *
 * This file contains the implementation of the methods declared in Player.h.
 * The Player class represents a player in a chess game and holds information about the player's color and associated user.
 * It provides methods to interact with and manipulate this data, allowing for player-specific actions and data retrieval.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#include "Player.h"

/**
 * @brief Constructs a Player object with an associated user and color.
 * @param user The associated user of the Player.
 * @param colour The color of the Player.
 * @param time The time left for the Player's game timer.
 */
Player::Player(User& user, Colour colour, std::chrono::minutes time) 
    : user(user), name(user.getUsername()), colour(colour), timeLeft(std::chrono::minutes(time)), 
        isInCheck(false), isResigning(false) {}

/**
 * @brief Retrieves the associated user of the Player.
 * @return A reference to the User object associated with the Player.
 */
User& Player::getUser() const {
    return user;
}

/**
 * @brief Retrieves the color of the Player.
 * @return The color of the Player.
 */
Colour Player::getColour() const {
    return colour;
}

/**
 * @brief Retrieves the name of the Player.
 * @return The name of the Player.
 */
const std::string& Player::getName() const {
    return name;
}

/**
 * @brief Retrieves the time left for the Player.
 * @return The time left for the Player's game timer.
 */
std::chrono::duration<double> Player::getTimeLeft() const {
    return timeLeft;
}

/**
 * @brief Sets the time left for the Player.
 * @param mins The amount of time to set as the Player's time left, in minutes.
*/
void Player::setTimeLeft(std::chrono::duration<double> mins) {
    timeLeft = mins;
}

/**
 * @brief Decreases the time left for the Player.
 *
 * @param duration The amount of time that has passed in the Player's turn.
 */
void Player::decrementTime(const std::chrono::duration<double>& duration) {
    if (duration < std::chrono::minutes(0)) {
        return;
    }

    timeLeft -= duration;

    if (timeLeft < std::chrono::minutes(0)) {
        timeLeft = std::chrono::minutes(0);
    }
}

/**
 * @brief Set the variable isInCheck to determine if the Player's King is in check.
 * @param isInCheck Bool determining whether the Player's King is in check or not.
 */
void Player::setIsInCheck(bool isInCheck) {
    this->isInCheck = isInCheck;
}

/**
 * @brief Retrieves the isInCheck variable for the Player to determine if their King is in check.
 * @return True if the Player's King is in check, false otherwise.
 */
bool Player::getIsInCheck() const {
    return isInCheck;
}

/**
 * @brief Retrieves the isResigning variable for the Player to determine if they are resigning.
 * @return True if the Player is resigning, false otherwise.
 */
bool Player::getIsResigning() const {
    return isResigning;
}

/**
 * @brief Set the variable isResigning to determine if the Player is resigning.
 */
void Player::setIsResigning() {
    this->isResigning = true;
}
