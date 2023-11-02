/**
 * @file test_Rook.cpp
 * @brief Unit tests for the Rook class.
 *
 * This file contains unit tests for the Rook class, including initial board movement, center board movement, edge board movement, obstructing pieces, and completely surrounded.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "Rook.h"
#include "Pawn.h"

/**
 * @brief Text fixture for Rook tests
 */
class RookTests : public ::testing::Test {
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

// Initial board movement for a Rook 
TEST_F(RookTests, InitialBoardMovement) {
    Position pos(3, 3); // Center position for a white rook 
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    
    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    placedRook->updateValidMoves(*board);

    // Define the expected moves for a rook placed in the center of an empty board 
    std::vector<Position> expectedMoves = {
        {3,0}, {3,1}, {3,2}, {3,4}, {3,5}, {3,6}, {3,7}, 
        {2,3}, {1,3}, {4,3}, {5,3}, 
    };

    auto validMoves = placedRook->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

// Center board movement for a Rook with some obstructing pieces 
TEST_F(RookTests, CenterBoardMovement) {
    Position pos(3, 3); 
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    
    // Placing some obstructing pieces
    auto obstructingPiece1 = std::make_unique<Pawn>(Colour::BLACK, Position(3, 5), PieceType::PAWN);
    auto obstructingPiece2 = std::make_unique<Pawn>(Colour::WHITE, Position(1, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(3, 5), std::move(obstructingPiece1)));
    EXPECT_TRUE(board->placePiece(Position(1, 3), std::move(obstructingPiece2)));
    
    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    placedRook->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {2,3}, {4,3}, {5,3}, 
        {3,0},{3,1}, {3,2}, {3,4}, {3, 5} 
    };

    auto validMoves = placedRook->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

// Movement for a Rook placed on the edge of the board 
TEST_F(RookTests, EdgeBoardMovement) {
    board->clearBoard();
    Position pos(0, 0);
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    
    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    placedRook->updateValidMoves(*board);

    std::vector<Position> expectedMoves;
    for(int i = 1; i < 8; i++) {
        expectedMoves.push_back({0, i});
        expectedMoves.push_back({i, 0});
    }
    
    auto validMoves = placedRook->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

// Movement for a Rook with some obstructing pieces on the board 
TEST_F(RookTests, ObstructingPieces) {
    Position pos(3, 3); 
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    
    // Placing some obstructing pieces
    auto obstructingPiece1 = std::make_unique<Pawn>(Colour::BLACK, Position(3, 5), PieceType::PAWN);
    auto obstructingPiece2 = std::make_unique<Pawn>(Colour::WHITE, Position(1, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(3, 5), std::move(obstructingPiece1)));
    EXPECT_TRUE(board->placePiece(Position(1, 3), std::move(obstructingPiece2)));
    
    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    placedRook->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {2,3}, {4,3}, {5,3}, 
        {3,0}, {3,1}, {3,2}, {3,4}, {3,5} 
    };
    
    auto validMoves = placedRook->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

// Checking rook's movement when completely surrounded by other pieces 
TEST_F(RookTests, CompletelySurrounded) {
    board->clearBoard();
    Position pos(3, 3); 
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    
    // Surround the rook with other pieces 
    for(int i = 2; i <= 4; i++) {
        for(int j = 2; j <= 4; j++) {
            if(i != 3 || j != 3) { // Don't replace the rook 
                auto piece = std::make_unique<Pawn>(Colour::WHITE, Position(i, j), PieceType::PAWN);
                EXPECT_TRUE(board->placePiece(Position(i, j), std::move(piece)));
            }
        }
    }
    
    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    placedRook->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {}; // No valid moves as the rook is completely surrounded 
    
    auto validMoves = placedRook->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
}