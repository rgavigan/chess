/**
 * @file test_Queen.cpp
 * @brief Unit tests for the Queen class.
 *
 * This file contains unit tests for the Queen class, including initial board movement, center board movement, edge board movement, obstructing pieces, and no movement possible.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "Queen.h"
#include "Pawn.h"

/**
 * @brief Test fixture for Queen tests
 */
class QueenTests : public ::testing::Test {
protected:
    ChessBoard* board;

    // Set up the board 
    void SetUp() override {
        board = new ChessBoard();
    }

    // Delete the board 
    void TearDown() override {
        delete board;
    }
};

TEST_F(QueenTests, InitialBoardMovement) {
    Position pos(3, 3); // Center position for a white queen 
    auto queen = std::make_unique<Queen>(Colour::WHITE, pos, PieceType::QUEEN);
    EXPECT_TRUE(board->placePiece(pos, std::move(queen)));
    
    auto* placedQueen = board->getPieceAtPosition(pos);
    EXPECT_NE(placedQueen, nullptr);
    placedQueen->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {3,0}, {3,1}, {3,2}, {3,4}, {3,5}, {3,6}, {3,7},
        {1,3}, {2,3}, {4,3}, {5,3},
        {1,1}, {2,2}, {4,4}, {5,5},
        {1,5}, {2,4}, {4,2}, {5,1}
    };

    auto validMoves = placedQueen->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(QueenTests, CenterBoardMovement) {
    Position pos(3, 3); // Center of the board for a white queen 
    auto queen = std::make_unique<Queen>(Colour::WHITE, pos, PieceType::QUEEN);
    EXPECT_TRUE(board->placePiece(pos, std::move(queen)));
    
    auto obstructingPiece1 = std::make_unique<Pawn>(Colour::BLACK, Position(3, 5), PieceType::PAWN);
    auto obstructingPiece2 = std::make_unique<Pawn>(Colour::WHITE, Position(1, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(3, 5), std::move(obstructingPiece1)));
    EXPECT_TRUE(board->placePiece(Position(1, 3), std::move(obstructingPiece2)));

    auto* placedQueen = board->getPieceAtPosition(pos);
    EXPECT_NE(placedQueen, nullptr);
    placedQueen->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {3,0}, {3,1}, {3,2}, {3,4}, {3,5},
        {2,3}, {4,3}, {5,3},
        {1,1}, {2,2}, {4,4}, {5,5},
        {1,5}, {2,4}, {4,2}, {5,1}
    };

    auto validMoves = placedQueen->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(QueenTests, EdgeBoardMovement) {
    board->clearBoard();
    Position pos(7, 7); // Edge of the board for a white queen 
    auto queen = std::make_unique<Queen>(Colour::WHITE, pos, PieceType::QUEEN);
    EXPECT_TRUE(board->placePiece(pos, std::move(queen)));
    
    auto* placedQueen = board->getPieceAtPosition(pos);
    EXPECT_NE(placedQueen, nullptr);
    placedQueen->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {7,1}, {7,2}, {7,3}, {7,4}, {7,5}, {7,6}, {7,0},
        {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {0,7},
        {1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {0,0}
    };
    
    auto validMoves = placedQueen->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(QueenTests, ObstructingPieces) {
    Position pos(3, 3); // Center of the board for a white queen 
    auto queen = std::make_unique<Queen>(Colour::WHITE, pos, PieceType::QUEEN);
    EXPECT_TRUE(board->placePiece(pos, std::move(queen)));
    
    auto obstructingPiece1 = std::make_unique<Pawn>(Colour::BLACK, Position(3, 5), PieceType::PAWN);
    auto obstructingPiece2 = std::make_unique<Pawn>(Colour::WHITE, Position(1, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(3, 5), std::move(obstructingPiece1)));
    EXPECT_TRUE(board->placePiece(Position(1, 3), std::move(obstructingPiece2)));

    auto* placedQueen = board->getPieceAtPosition(pos);
    EXPECT_NE(placedQueen, nullptr);
    placedQueen->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {3,0}, {3,1}, {3,2}, {3,4}, {3,5},
        {2,3}, {4,3}, {5,3},
        {1,1}, {2,2}, {4,4}, {5,5},
        {1,5}, {2,4}, {4,2}, {5,1}
    };

    auto validMoves = placedQueen->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(QueenTests, NoMovementPossible) {
    Position pos(7, 4); // White queen starting position 
    
    auto* queen = board->getPieceAtPosition(pos);
    EXPECT_NE(queen, nullptr);
    queen->updateValidMoves(*board);

    // Queen can't move on first turn 
    std::vector<Position> expectedMoves = {};
    
    auto validMoves = queen->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}