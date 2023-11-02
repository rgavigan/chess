/**
 * @file test_User.cpp
 * @brief Unit tests for the User class.
 *
 * This file contains unit tests for the User class, including basic user information,
 * incrementing user statistics, and getting user statistics.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */

#include <gtest/gtest.h>

#include "User.h"

/**
 * @brief Test fixture for User tests.
 */
class UserTests : public testing::Test {
protected:
    User* user;
    int expectedWins = 5;
    int expectedLosses = 3;
    int expectedDraws = 2;

    // Initialize user instance 
    void SetUp() override {
        user = new User("Alice");
    }

    // Delete user instance 
    void TearDown() override {
        delete user;
    }
};

/**
 * @brief Test case for basic user information.
 * Test the creation of a User object and retrieval of basic user information.
 */
TEST_F(UserTests, BasicUserInfo) {
    EXPECT_EQ(user->getUsername(), "Alice");
    EXPECT_EQ(user->getWins(), 0);
    EXPECT_EQ(user->getLosses(), 0);
    EXPECT_EQ(user->getDraws(), 0);
}

/**
 * @brief Test case for incrementing user statistics.
 * Test incrementing user statistics (wins, losses, and draws).
 */
TEST_F(UserTests, IncrementStats) {
    user->incrementWins();
    user->incrementLosses();
    user->incrementDraws();

    EXPECT_EQ(user->getWins(), 1);
    EXPECT_EQ(user->getLosses(), 1);
    EXPECT_EQ(user->getDraws(), 1);
}

/**
 * @brief Test case for getting user statistics.
 * Test getting user statistics (wins, losses, and draws) after initializing them.
 */
TEST_F(UserTests, GetUserStatsSuccess) {
    user->initializeStats(expectedWins, expectedLosses, expectedDraws);

    EXPECT_EQ(user->getUsername(), "Alice");
    EXPECT_EQ(user->getWins(), expectedWins);
    EXPECT_EQ(user->getLosses(), expectedLosses); 
    EXPECT_EQ(user->getDraws(), expectedDraws);
}

/**
 * @brief Test case for updating username
 */
TEST_F(UserTests, UpdateUsername) {
    user->setUsername("Bob");

    EXPECT_EQ(user->getUsername(), "Bob");
}

/**
 * @brief Test case for updating timer
 */
TEST_F(UserTests, UpdateTimer) {
    user->setTimer(std::chrono::minutes(30));
    EXPECT_EQ(user->getTimer(), std::chrono::minutes(30));
}