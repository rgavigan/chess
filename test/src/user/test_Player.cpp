/**
 * @file test_Player.cpp
 * @brief Unit tests for the Player class.
 *
 * This file contains unit tests for the Player class, including basic player information,
 * updating player time, setting the "is in check" flag, handling invalid time updates,
 * handling negative time updates, and setting the resigning flag.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */

#include <gtest/gtest.h>

#include "Player.h"
#include "User.h"

/**
 * @brief Test fixture for Player tests.
 */
class PlayerTests : public testing::Test {
protected:
    User *user;
    Player *player;

    // Initialize player & user instances 
    void SetUp() override {
        user = new User("Alice");
        player = new Player(*user, Colour::WHITE, std::chrono::minutes(30));
    }

    // Delete player & user instances 
    void TearDown() override {
        delete player;
        delete user;
    }
};

/**
 * @brief Test case for basic player information.
 * Test the creation of a Player object and retrieval of basic player information.
 */
TEST_F(PlayerTests, BasicPlayerInfo) {
    EXPECT_EQ(player->getName(), "Alice");
    EXPECT_EQ(player->getColour(), Colour::WHITE);
    EXPECT_EQ(player->getTimeLeft(), std::chrono::minutes(30));
    EXPECT_FALSE(player->getIsInCheck());
}

/**
 * @brief Test case for updating player time.
 * Test updating the time left for a player->
 */
TEST_F(PlayerTests, UpdateTime) {
    player->decrementTime(std::chrono::minutes(5));
    EXPECT_EQ(player->getTimeLeft(), std::chrono::minutes(25));
}

/**
 * @brief Test case for setting the "is in check" flag.
 * Test setting and retrieving the "is in check" flag for a player->
 */
TEST_F(PlayerTests, SetIsInCheck) {
    player->setIsInCheck(true);
    EXPECT_TRUE(player->getIsInCheck());

    player->setIsInCheck(false);
    EXPECT_FALSE(player->getIsInCheck());
}

/**
 * @brief Test case for handling invalid time updates.
 * Test handling time updates that would result in negative time.
 */
TEST_F(PlayerTests, InvalidTimeUpdate) {
    // Decrementing more time than available 
    player->decrementTime(std::chrono::minutes(40));
    EXPECT_EQ(player->getTimeLeft(), std::chrono::minutes(0));
}

/**
 * @brief Test case for handling negative time updates.
 * Test handling time updates with negative time values.
 */
TEST_F(PlayerTests, NegativeTime) {
    // Decrementing with negative time 
    player->decrementTime(std::chrono::minutes(-5));
    EXPECT_EQ(player->getTimeLeft(), std::chrono::minutes(30));
}

/**
 * @brief Test case for setting the resigning flag.
 * Test setting and retrieving the resigning flag for a player->
 */
TEST_F(PlayerTests, ResigningFlag) {
    EXPECT_FALSE(player->getIsResigning()); // Initially, not resigning 

    player->setIsResigning();
    EXPECT_TRUE(player->getIsResigning()); // Now resigning 
}