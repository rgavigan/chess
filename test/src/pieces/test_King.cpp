/**
 * @file test_King.cpp
 * @brief Unit tests for the King class.
 *
 * This file contains unit tests for the King class, including initial board movement, edge board movement, center board movement, obstructing pieces, and castling.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "King.h"
#include "Pawn.h"
#include "Rook.h"
#include "Queen.h"

/**
 * @brief Test fixture for King tests
 */
class KingTests : public ::testing::Test {
protected:
    ChessBoard* board;

    // Set up board 
    void SetUp() override {
        board = new ChessBoard();
        board->clearBoard();
    }

    // Delete board 
    void TearDown() override {
        delete board;
    }
};


TEST_F(KingTests, InitialBoardMovement) {
    Position pos(7, 3);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    std::vector<Position> expectedMoves = {
        {6, 2}, {6, 3}, {6, 4}, {7, 2}, {7, 4}
    };
    
    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for (const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KingTests, EdgeBoardMovement) {
    Position pos(0, 0);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    std::vector<Position> expectedMoves = {
        {0, 1}, {1, 0}, {1, 1}
    };
    
    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for (const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KingTests, CenterBoardMovement) {
    Position pos(3, 3);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    std::vector<Position> expectedMoves = {
        {2, 2}, {2, 4}, {3, 2}, {3, 4}, {4, 2}, {4, 4}, {4, 3}, {2, 3}
    };
    
    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KingTests, ObstructingPieces) {
    Position pos(3, 3);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    auto obstructingPiece1 = std::make_unique<Pawn>(Colour::BLACK, Position(4, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(4, 3), std::move(obstructingPiece1)));

    auto obstructingPiece2 = std::make_unique<Pawn>(Colour::WHITE, Position(2, 3), PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(Position(2, 3), std::move(obstructingPiece2)));

    std::vector<Position> expectedMoves = {
        {2, 2}, {2, 4}, {3, 2}, {3, 4}, {4, 2}, {4, 4}, {4, 3}
    };

    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KingTests, Castling) {
    Position pos(7, 4);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    Position leftRookPos{7,0};
    auto leftRook = std::make_unique<Rook>(Colour::WHITE, leftRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(leftRookPos, std::move(leftRook)));

    Position rightRookPos{7,7};
    auto rightRook = std::make_unique<Rook>(Colour::WHITE, rightRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(rightRookPos, std::move(rightRook)));

    std::vector<Position> expectedMoves = {
        {7, 2}, {7, 6}, // potential castling moves 
        {7, 3}, {7, 5}, {6, 4}, {6, 3}, {6, 5}
    };

    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}

TEST_F(KingTests, CastlingAfterMovement) {
    Position pos(7, 4);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    Position leftRookPos{7,0};
    auto leftRook = std::make_unique<Rook>(Colour::WHITE, leftRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(leftRookPos, std::move(leftRook)));

    Position rightRookPos{7,7};
    auto rightRook = std::make_unique<Rook>(Colour::WHITE, rightRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(rightRookPos, std::move(rightRook)));

    EXPECT_TRUE(board->movePiece(rightRookPos, Position(7,6)));
    EXPECT_TRUE(board->movePiece(Position(7,6), rightRookPos));
    
    std::vector<Position> expectedMoves = {
        {7,2}, {7, 3}, {7, 5}, {6, 4}, {6, 3}, {6, 5}  // can castle only with the left rook since the right rook has moved 
    };

    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }

    EXPECT_TRUE(board->movePiece(pos, Position(6,4)));
    EXPECT_TRUE(board->movePiece(Position(6,4), pos));

    expectedMoves = {
        {7, 3}, {7, 5}, {6, 4}, {6, 3}, {6, 5}  // no castling moves as the king has moved 
    };

    validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }

      
}

TEST_F(KingTests, CastlingThroughCheck) {
    Position pos(7, 4);
    auto king = std::make_unique<King>(Colour::WHITE, pos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(pos, std::move(king)));

    auto* placedKing = board->getPieceAtPosition(pos);
    EXPECT_NE(placedKing, nullptr);

    Position leftRookPos{7,0};
    auto leftRook = std::make_unique<Rook>(Colour::WHITE, leftRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(leftRookPos, std::move(leftRook)));

    Position rightRookPos{7,7};
    auto rightRook = std::make_unique<Rook>(Colour::WHITE, rightRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(rightRookPos, std::move(rightRook)));

    Position opponentQueenPos{2,0};
    auto opponentQueen = std::make_unique<Queen>(Colour::BLACK, opponentQueenPos, PieceType::QUEEN);
    EXPECT_TRUE(board->placePiece(opponentQueenPos, std::move(opponentQueen)));

    std::vector<Position> expectedMoves = {
        {7, 2}, {7, 3}, {6, 3}, {6, 5}  // Can only castle with left rook 
    };

    auto validMoves = placedKing->getValidMoves();
    EXPECT_EQ(validMoves.size(), expectedMoves.size());

    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(validMoves.begin(), validMoves.end(), move), validMoves.end());
    }
}
