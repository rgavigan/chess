/**
 * @file test_Bishop.cpp
 * @brief Unit tests for the Bishop class.
 *
 * This file contains unit tests for the Bishop class, center board movement, corner board movement, and obstructing pieces.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm> 

#include "Bishop.h"

/**
 * @brief Text fixture for the Bishop class
 */
class BishopTests : public ::testing::Test {
protected:
    ChessBoard* board;
    Position* centerPos;
    Position* cornerPos;
    std::unique_ptr<Bishop> centerBishop;
    std::unique_ptr<Bishop> cornerBishop;

    // Initialize board and bishops 
    void SetUp() override {
        board = new ChessBoard();
        centerPos = new Position(3, 3);  // Center of the board 
        cornerPos = new Position(0, 0);  // Corner of the board 
        centerBishop = std::make_unique<Bishop>(Colour::WHITE, *centerPos, PieceType::BISHOP);
        cornerBishop = std::make_unique<Bishop>(Colour::WHITE, *cornerPos, PieceType::BISHOP);
    }

    // Delete pointers 
    void TearDown() override {
        delete board;
        delete centerPos;
        delete cornerPos;
    }
};



TEST_F(BishopTests, CenterBoardMovement) {
    EXPECT_TRUE(board->placePiece(*centerPos, std::move(centerBishop)));
    
    auto* placedBishop = board->getPieceAtPosition(*centerPos);
    EXPECT_NE(placedBishop, nullptr);
    placedBishop->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {1,1}, {2,2}, {4,4}, {5,5}, {1,5}, {2,4}, {4,2}, {5,1}
    };
    auto validMoves = placedBishop->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(BishopTests, CornerBoardMovement) {
    board->clearBoard();
    EXPECT_TRUE(board->placePiece(*cornerPos, std::move(cornerBishop)));
    
    auto* placedBishop = board->getPieceAtPosition(*cornerPos);
    EXPECT_NE(placedBishop, nullptr);
    placedBishop->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {7,7}
    };
    auto validMoves = placedBishop->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(BishopTests, ObstructingPieces) {
    board->initializeBoard();
    EXPECT_TRUE(board->placePiece(*centerPos, std::move(centerBishop)));
    
    auto* placedBishop = board->getPieceAtPosition(*centerPos);
    EXPECT_NE(placedBishop, nullptr);
    placedBishop->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {
        {1,1}, {2,2}, {2,4}, {1,5}, {4,2}, {5,1}, {4,4}, {5,5}
    };
    
    auto validMoves = placedBishop->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}