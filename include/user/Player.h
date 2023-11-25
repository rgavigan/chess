/**
 * @file Player.h
 * @brief Declaration of the Player class.
 *
 * This class represents a player in a chess game and holds information about the player's color and associated user.
 * It provides methods to interact with and manipulate this data, allowing for player-specific actions and data retrieval.
 *
 * @author Brad McGlynn
 * @date September 28th, 2023
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <chrono>
#include "User.h"
#include "DataUtil.h"

class Player {
public:
    /** Constructor to initialize the player with a color, name, and time left on the timer. */
    Player(User& user, Colour colour, std::chrono::minutes time);

    /** Method to get the User associated with the player. */
    User& getUser() const;
    
    /** Method to get the color of the player. */
    Colour getColour() const;

    /** Method to get the name of the player. */
    const std::string& getName() const;

    /** Method to get the time left for the player. */
    std::chrono::duration<double> getTimeLeft() const;

    /** Sets the time left for the Player. */
    void setTimeLeft(std::chrono::duration<double> mins);

    /** Method to decrement the player's time left by a certain duration. */
    void decrementTime(const std::chrono::duration<double>& duration);

    /** Method to set whether the player's king is in check. */
    void setIsInCheck(bool isInCheck);

    /** Method to get whether the player's king is in check. */
    bool getIsInCheck() const;

    /** Method to get whether the player is resigning. */
    bool getIsResigning() const;

    /** Method to set whether the player is resigning. */
    void setIsResigning();

private:
    Colour colour; // The colour assigned to the player
    std::string name; // The name of the player
    std::chrono::duration<double> timeLeft; // The time left for the player
    bool isInCheck; // Whether the player's king is in check
    bool isResigning; // Whether the player is resigning
    User& user; // The user associated with the player
};

#endif // PLAYER_H
