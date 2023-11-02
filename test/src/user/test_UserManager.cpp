/**
 * @file test_UserManager.cpp
 * @brief Unit tests for the UserManager class.
 *
 * This file contains unit tests for the UserManager class, including createUser, authenticateUser, getUserStats,
 * and updateUserStats methods.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */

#include <gtest/gtest.h>

#include "User.h" 
#include "UserManager.h" 

/**
 * @brief Test fixture for UserManager tests.
 */
class UserManagerTests : public testing::Test {
protected:
    UserManager* userManager;
    User* user;
    int wins = 0, losses = 0, draws = 0;

    // Initialize user & user manager instances 
    void SetUp() override {
        userManager = new UserManager(":memory:");
        user= new User("Alice");
    }

    // Delete user & user manager instance 
    void TearDown() override {
        delete userManager;
        delete user;
    }
};

/**
 * @brief Test case for createUser method.
 * Test the successful creation of a user.
 */
TEST_F(UserManagerTests, CreateUserSuccess) {
    EXPECT_TRUE(userManager->createUser("Alice", "password"));
}

/**
 * @brief Test case for createUser method.
 * Test the creation of a duplicate user, which should fail.
 */
TEST_F(UserManagerTests, CreateUserDuplicate) {
    // Attempt to create a duplicate user 
    EXPECT_TRUE(userManager->createUser("Alice", "password"));
    EXPECT_FALSE(userManager->createUser("Alice", "password"));
}

/**
 * @brief Test case for authenticateUser method.
 * Test the successful authentication of a user.
 */
TEST_F(UserManagerTests, AuthenticateUserSuccess) {
    // Create a user 
    EXPECT_TRUE(userManager->createUser("Alice", "password"));

    // Authenticate the user 
    EXPECT_TRUE(userManager->authenticateUser("Alice", "password"));
}

/**
 * @brief Test case for authenticateUser method.
 * Test authentication with an invalid password, which should fail.
 */
TEST_F(UserManagerTests, AuthenticateUserInvalidPassword) {
    // Create a user 
    EXPECT_TRUE(userManager->createUser("Alice", "password"));

    // Attempt to authenticate with an invalid password 
    EXPECT_FALSE(userManager->authenticateUser("Alice", "wrong_password"));
}

/**
 * @brief Test case for authenticateUser method.
 * Test authentication of a non-existent user, which should fail.
 */
TEST_F(UserManagerTests, AuthenticateUserNonExistentUser) {
    // Attempt to authenticate a non-existent user 
    EXPECT_FALSE(userManager->authenticateUser("Bob", "password"));
}

/**
 * @brief Test case for getUserStats method.
 * Test retrieving statistics of an existing user.
 */
TEST_F(UserManagerTests, GetUserStatsSuccess) {
    EXPECT_TRUE(userManager->createUser("Alice", "password"));
    EXPECT_TRUE(userManager->getUserStats("Alice", wins, losses, draws));
    
    user->initializeStats(wins, losses, draws);
    EXPECT_EQ(user->getWins(), wins);
    EXPECT_EQ(user->getLosses(), losses);
    EXPECT_EQ(user->getDraws(), draws);
}

/**
 * @brief Test case for getUserStats method.
 * Test retrieving statistics of a non-existent user, which should fail.
 */
TEST_F(UserManagerTests, GetUserStatsNonExistentUser) {
    // Attempt to get stats for a non-existent user 
    EXPECT_FALSE(userManager->getUserStats("Bob", wins, losses, draws));
}

/**
 * @brief Test case for updateUserStats method.
 * Test updating the statistics of an existing user.
 */
TEST_F(UserManagerTests, UpdateUserStatsSuccess) {
    EXPECT_TRUE(userManager->createUser("Alice", "password"));

    // Update user stats 
    EXPECT_TRUE(userManager->updateUserStats("Alice", 5, 3, 2));
    EXPECT_TRUE(userManager->getUserStats("Alice", wins, losses, draws));
    
    user->initializeStats(wins, losses, draws);
    EXPECT_EQ(user->getWins(), wins);
    EXPECT_EQ(user->getLosses(), losses);
    EXPECT_EQ(user->getDraws(), draws);
}

/**
 * @brief Test case for updateUserStats method.
 * Test updating the statistics of a non-existent user, which should fail.
 */
TEST_F(UserManagerTests, UpdateUserStatsNonExistentUser) {
    // Attempt to update stats for a non-existent user 
    EXPECT_FALSE(userManager->updateUserStats("Bob", 5, 3, 2));
}