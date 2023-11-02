/**
 * @file test_Pawn.cpp
 * @brief Unit tests for the Pawn class.
 *
 * This file contains unit tests for the Pawn class, including initial board movement, center board movement, edge board movement, obstructing pieces, en passant, and diagonal capture.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>

#include "Pawn.h"

/**
 * @brief Test fixture for Pawn tests
 */
class PawnTests : public ::testing::Test {
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

TEST_F(PawnTests, InitialBoardMovement) {
    Position pos(6, 3);
    auto pawn = std::make_unique<Pawn>(Colour::WHITE, pos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(pos, std::move(pawn)));
    
    auto* placedPawn = board->getPieceAtPosition(pos);
    EXPECT_NE(placedPawn, nullptr);
    placedPawn->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {{5,3}, {4,3}};
    
    auto validMoves = placedPawn->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(PawnTests, CenterBoardMovement) {
    Position pos(3, 3);
    auto pawn = std::make_unique<Pawn>(Colour::WHITE, pos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(pos, std::move(pawn)));
    
    auto* placedPawn = board->getPieceAtPosition(pos);
    EXPECT_NE(placedPawn, nullptr);
    placedPawn->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {{2,3}};
    
    auto validMoves = placedPawn->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(PawnTests, EdgeBoardMovement) {
    Position pos(6, 0);
    auto pawn = std::make_unique<Pawn>(Colour::WHITE, pos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(pos, std::move(pawn)));
    
    auto* placedPawn = board->getPieceAtPosition(pos);
    EXPECT_NE(placedPawn, nullptr);
    placedPawn->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {{5,0}, {4,0}};
    
    auto validMoves = placedPawn->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(PawnTests, ObstructingPieces) {
    Position pos(3, 3);
    auto pawn = std::make_unique<Pawn>(Colour::WHITE, pos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(pos, std::move(pawn)));

    Position obstructingPos(2, 3);
    auto obstructingPiece = std::make_unique<Pawn>(Colour::BLACK, obstructingPos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(obstructingPos, std::move(obstructingPiece)));
    
    auto* placedPawn = board->getPieceAtPosition(pos);
    EXPECT_NE(placedPawn, nullptr);
    placedPawn->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {};
    
    auto validMoves = placedPawn->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
}

TEST_F(PawnTests, EnPassant) {
    Position whitePawnPos(3, 3);
    auto whitePawn = std::make_unique<Pawn>(Colour::WHITE, whitePawnPos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(whitePawnPos, std::move(whitePawn)));

    Position blackPawnPos(1, 2);
    auto blackPawn = std::make_unique<Pawn>(Colour::BLACK, blackPawnPos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(blackPawnPos, std::move(blackPawn)));

    EXPECT_TRUE(board->movePiece(blackPawnPos, Position{3,2}));

    auto* placedWhitePawn = board->getPieceAtPosition(whitePawnPos);
    EXPECT_NE(placedWhitePawn, nullptr);

    std::vector<Position> expectedMoves = {{2,3}, {2,2}};
    
    auto validMoves = placedWhitePawn->getValidMoves();

    EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), expectedMoves.front()), validMoves.end());
}

TEST_F(PawnTests, DiagonalCapture) {
    Position whitePawnPos(3, 3);
    auto whitePawn = std::make_unique<Pawn>(Colour::WHITE, whitePawnPos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(whitePawnPos, std::move(whitePawn)));

    Position blackPawnPos(2, 2);  // Diagonal to the white pawn 
    auto blackPawn = std::make_unique<Pawn>(Colour::BLACK, blackPawnPos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(blackPawnPos, std::move(blackPawn)));

    auto* placedWhitePawn = board->getPieceAtPosition(whitePawnPos);
    EXPECT_NE(placedWhitePawn, nullptr);
    placedWhitePawn->updateValidMoves(*board);

    std::vector<Position> expectedMoves = {{2,3}, {2,2}};  // Forward move and diagonal capturing move 
    
    auto validMoves = placedWhitePawn->getValidMoves();

    EXPECT_EQ(validMoves.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}