/**
 * @file test_Knight.cpp
 * @brief Unit tests for the Knight class.
 *
 * This file contains unit tests for the Knight class, including center board movement, corner board movement, edge board movement, and obstructing pieces.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm> 

#include "Knight.h"

/**
 * @brief Test fixture for Knight tests
 */
class KnightTests : public ::testing::Test {
protected:
    ChessBoard* board;

    // Set up board 
    void SetUp() override {
        board = new ChessBoard();
    }

    // Delete board 
    void TearDown() override {
        delete board;
    }
};

TEST_F(KnightTests, CenterBoardMovement) {
    Position pos(3, 3);
    auto knight = std::make_unique<Knight>(Colour::WHITE, pos, PieceType::KNIGHT);
    EXPECT_TRUE(board->placePiece(pos, std::move(knight)));

    auto* placedKnight = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKnight, nullptr);
    
    std::vector<Position> expectedMoves = {
        {1,2}, {1,4}, {2,1}, {2,5}, {4,1}, {4,5}, {5,2}, {5,4}
    };

    auto validMoves = placedKnight->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KnightTests, CornerBoardMovement) {
    Position pos(0, 0);
    auto knight = std::make_unique<Knight>(Colour::WHITE, pos, PieceType::KNIGHT);
    EXPECT_TRUE(board->placePiece(pos, std::move(knight)));

    auto* placedKnight = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKnight, nullptr);
    
    std::vector<Position> expectedMoves = {
        {1,2}, {2,1}
    };

    auto validMoves = placedKnight->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KnightTests, EdgeBoardMovement) {
    Position pos(0, 3);
    auto knight = std::make_unique<Knight>(Colour::WHITE, pos, PieceType::KNIGHT);
    EXPECT_TRUE(board->placePiece(pos, std::move(knight)));

    auto* placedKnight = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKnight, nullptr);
    
    std::vector<Position> expectedMoves = {
        {1,1}, {1,5}, {2,2}, {2,4}
    };

    auto validMoves = placedKnight->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KnightTests, ObstructingPieces) {
    board->initializeBoard();
    Position pos(3, 3);
    auto knight = std::make_unique<Knight>(Colour::WHITE, pos, PieceType::KNIGHT);
    EXPECT_TRUE(board->placePiece(pos, std::move(knight)));

    auto* placedKnight = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKnight, nullptr);

    // Knight's movement isn't affected by other pieces.
    std::vector<Position> expectedMoves = {
        {1,2}, {1,4}, {2,1}, {2,5}, {4,1}, {4,5}, {5,2}, {5,4}
    };

    auto validMoves = placedKnight->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}
