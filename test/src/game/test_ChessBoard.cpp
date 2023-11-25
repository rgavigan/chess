/**
 * @file test_ChessBoard.cpp
 * @brief Unit tests for the ChessBoard class.
 *
 * This file contains unit tests for the ChessBoard class, including the constructor, initializeBoard, getPieceAtPosition, movePiece, isValidMove, isSpaceEmpty, isSpaceFriendly, isSpaceEnemy, isSquareSafe, getKingPosition, getPiecesOfColour, getBoardStateAsString, isDeadPosition, isPositionOnBoard, isPawnEligibleForEnPassant, clearBoard, placePiece, initializeBoardFromString methods.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>

#include "ChessBoard.h"
#include "Rook.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"

/**
 * @brief Test fixture for ChessBoard tests.
 */
class ChessBoardTests : public testing::Test {
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

TEST_F(ChessBoardTests, Constructor) {
    std::string boardState = board->getBoardStateAsString();
    std::string expectedBoardState = "r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\n........\n........\nP-P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);
}

TEST_F(ChessBoardTests, InitializeBoard) {
    board->initializeBoard();
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 0})->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 7})->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(board->getPieceAtPosition(Position{7, 0})->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(board->getPieceAtPosition(Position{7, 7})->getPieceType(), PieceType::ROOK);
}

TEST_F(ChessBoardTests, GetPieceAtPosition) {
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 0})->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(board->getPieceAtPosition(Position{1, 0})->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 1})->getPieceType(), PieceType::KNIGHT);
    EXPECT_EQ(board->getPieceAtPosition(Position{2, 2}), nullptr);
}

TEST_F(ChessBoardTests, MovePiece) {
    // Move a pawn forward 
    EXPECT_TRUE(board->movePiece(Position{1, 0}, Position{2, 0}));
    EXPECT_EQ(board->getPieceAtPosition(Position{2, 0})->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(board->getPieceAtPosition(Position{2, 0})->getColour(), Colour::BLACK);
    EXPECT_EQ(board->getPieceAtPosition(Position{1, 0}), nullptr);

    // Try moving a piece to an occupied space by a friendly piece 
    EXPECT_FALSE(board->movePiece(Position{0, 0}, Position{2, 0}));

    // Try moving a piece out of bounds 
    EXPECT_FALSE(board->movePiece(Position{0, 0}, Position{8, 8}));

    // Capture an opponent's piece 
    Position pos(5, 0);
    auto pawn = std::make_unique<Pawn>(Colour::BLACK, pos, PieceType::PAWN);
    EXPECT_TRUE(board->placePiece(pos, std::move(pawn)));
    
    EXPECT_TRUE(board->movePiece(Position{6, 1}, Position{5, 0}));
    EXPECT_EQ(board->getPieceAtPosition(Position{5, 0})->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(board->getPieceAtPosition(Position{5, 0})->getColour(), Colour::WHITE);
    EXPECT_EQ(board->getPieceAtPosition(Position{6, 1}), nullptr);
    
    // Move a knight in an L-shape 
    EXPECT_TRUE(board->movePiece(Position{0, 1}, Position{2, 2}));
    EXPECT_EQ(board->getPieceAtPosition(Position{2, 2})->getPieceType(), PieceType::KNIGHT);
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 1}), nullptr);

    // Try moving a bishop in a straight line (invalid move for a bishop) 
    EXPECT_FALSE(board->movePiece(Position{0, 2}, Position{0, 4}));

    // Move a bishop diagonally 
    EXPECT_TRUE(board->movePiece(Position{1, 3}, Position{2, 3}));
    EXPECT_TRUE(board->movePiece(Position{0, 2}, Position{2, 4}));
    EXPECT_EQ(board->getPieceAtPosition(Position{2, 4})->getPieceType(), PieceType::BISHOP);
    EXPECT_EQ(board->getPieceAtPosition(Position{0, 2}), nullptr);
}

TEST_F(ChessBoardTests, IsValidMove) {
    // Valid move for a pawn 
    EXPECT_TRUE(board->isValidMove(Position{1, 0}, Position{2, 0}));

    // Invalid move for a pawn (moving backwards) 
    EXPECT_FALSE(board->isValidMove(Position{2, 0}, Position{1, 0}));

    // Valid move for a rook 
    Position pos(3, 3); // Center position for a white rook 
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));
    EXPECT_TRUE(board->isValidMove(pos, Position{3, 7}));

    // Invalid move for a rook (diagonal) 
    EXPECT_FALSE(board->isValidMove(pos, Position{4, 4}));

    // Moving to a space occupied by a friendly piece 
    EXPECT_FALSE(board->isValidMove(Position{0, 0}, Position{1, 0}));

    // Moving out of bounds 
    EXPECT_FALSE(board->isValidMove(Position{0, 0}, Position{8, 8}));

    // Invalid move for a queen (moving like a knight) 
    EXPECT_FALSE(board->isValidMove(Position{0, 3}, Position{2, 4}));

    // Valid move for a queen (moving diagonally) 
    EXPECT_TRUE(board->movePiece(Position{1, 4}, Position{2, 4}));
    EXPECT_TRUE(board->isValidMove(Position{0, 3}, Position{3, 6}));

    // Check for pawn's first move (can move two squares forward) 
    EXPECT_TRUE(board->isValidMove(Position{1, 0}, Position{3, 0}));

    // Check for pawn's subsequent move (cannot move two squares forward) 
    board->movePiece(Position{1, 0}, Position{2, 0});
    EXPECT_FALSE(board->isValidMove(Position{2, 0}, Position{4, 0}));

    board->clearBoard(); // Clearing board to give king room to move 

    Position kingPos(7, 4); // Center position for a white King 
    auto king = std::make_unique<King>(Colour::WHITE, kingPos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(kingPos, std::move(king)));

    // Invalid move for a king (moving more than one square) 
    EXPECT_FALSE(board->isValidMove(kingPos, Position{5, 4}));

    // Valid move for a king (moving one square) 
    EXPECT_TRUE(board->isValidMove(kingPos, Position{6, 4}));

    Position leftRookPos{6,0}; // Opponent rook to test that king can't move into check 
    auto leftRook = std::make_unique<Rook>(Colour::BLACK, leftRookPos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(leftRookPos, std::move(leftRook)));

    // Invalid move for a king (moving into check) 
    EXPECT_FALSE(board->isValidMove(kingPos, Position{6,4}));
}


TEST_F(ChessBoardTests, IsSpaceEmpty) {
    // Check if a space occupied by a piece is empty 
    EXPECT_FALSE(board->isSpaceEmpty(Position{0, 0}));

    // Check if an empty space is empty 
    EXPECT_TRUE(board->isSpaceEmpty(Position{2, 2}));
}

TEST_F(ChessBoardTests, IsSpaceFriendly) {
    // Check if a space occupied by a friendly piece is friendly 
    EXPECT_FALSE(board->isSpaceFriendly(Position{0, 0}, Colour::WHITE));

    // Check if a space occupied by an enemy piece is friendly 
    EXPECT_TRUE(board->isSpaceFriendly(Position{0, 0}, Colour::BLACK));

    // Check if an empty space is friendly 
    EXPECT_FALSE(board->isSpaceFriendly(Position{2, 2}, Colour::WHITE));
}

TEST_F(ChessBoardTests, IsSpaceEnemy) {
    // Check if a space occupied by an enemy piece is an enemy 
    EXPECT_TRUE(board->isSpaceEnemy(Position{0, 0}, Colour::WHITE));

    // Check if a space occupied by a friendly piece is an enemy 
    EXPECT_FALSE(board->isSpaceEnemy(Position{0, 0}, Colour::BLACK));

    // Check if an empty space is an enemy 
    EXPECT_FALSE(board->isSpaceEnemy(Position{2, 2}, Colour::BLACK));
}


TEST_F(ChessBoardTests, IsSquareSafe) {
    board->clearBoard();

    // Place a white king 
    Position kingPos(7, 4); 
    auto king = std::make_unique<King>(Colour::WHITE, kingPos, PieceType::KING);
    EXPECT_TRUE(board->placePiece(kingPos, std::move(king)));

    // Place a black bishop 
    Position bishopPos(2, 0); 
    auto bishop = std::make_unique<Bishop>(Colour::BLACK, bishopPos, PieceType::BISHOP);
    EXPECT_TRUE(board->placePiece(bishopPos, std::move(bishop)));

    // Check if a spot is safe for a king to move to 
    EXPECT_FALSE(board->isSquareSafe(Position{6, 4}, Colour::WHITE));
    EXPECT_TRUE(board->isSquareSafe(Position{6, 3}, Colour::WHITE));
}

TEST_F(ChessBoardTests, GetKingPosition) {
    // Check if the king's position is returned correctly 
    Position whiteKingPos(7, 4);
    Position blackKingPos(0, 4);
    EXPECT_EQ(board->getKingPosition(Colour::WHITE), whiteKingPos);
    EXPECT_EQ(board->getKingPosition(Colour::BLACK), blackKingPos);
}

TEST_F(ChessBoardTests, GetPiecesOfColour) {
    // Check if the pieces of a colour are returned correctly 
    std::vector<Piece*> whitePieces = board->getPiecesOfColour(Colour::WHITE);
    EXPECT_EQ(whitePieces.size(), 16);
    std::vector<Piece*> blackPieces = board->getPiecesOfColour(Colour::BLACK);
    EXPECT_EQ(blackPieces.size(), 16);

    board->clearBoard();

    whitePieces = board->getPiecesOfColour(Colour::WHITE);
    EXPECT_TRUE(whitePieces.empty());
    blackPieces = board->getPiecesOfColour(Colour::BLACK);
    EXPECT_TRUE(blackPieces.empty());
}


TEST_F(ChessBoardTests, GetBoardStateAsString) {
    std::string boardState;
    std::string expectedBoardState;

    boardState = board->getBoardStateAsString();
    expectedBoardState = "r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\n........\n........\nP-P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);

    board->movePiece(Position{1, 0}, Position{3, 0});
    boardState = board->getBoardStateAsString();
    expectedBoardState = "r-n-b-q-k-b-n-r-\n.p-p-p-p-p-p-p-\n........\np-.......\n........\n........\nP-P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);

    board->movePiece(Position{6, 0}, Position{4, 0});
    boardState = board->getBoardStateAsString();
    expectedBoardState = "r-n-b-q-k-b-n-r-\n.p-p-p-p-p-p-p-\n........\np-.......\nP-.......\n........\n.P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);

    board->movePiece(Position{0, 1}, Position{2, 0});
    boardState = board->getBoardStateAsString();
    expectedBoardState = "r-.b-q-k-b-n-r-\n.p-p-p-p-p-p-p-\nn-.......\np-.......\nP-.......\n........\n.P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);

    board->movePiece(Position{7, 1}, Position{5, 0});
    boardState = board->getBoardStateAsString();
    expectedBoardState = "r-.b-q-k-b-n-r-\n.p-p-p-p-p-p-p-\nn-.......\np-.......\nP-.......\nN-.......\n.P-P-P-P-P-P-P-\nR-.B-Q-K-B-N-R-\n";
    EXPECT_EQ(boardState, expectedBoardState);
}

TEST_F(ChessBoardTests, IsDeadPosition) {
    EXPECT_FALSE(board->isDeadPosition());

    board->clearBoard();

    std::string expectedBoardState = ".......k-\n......n-\n........\n........\n........\n........\n........\n.......K-\n";
    board->initializeBoardFromString(expectedBoardState);
    EXPECT_TRUE(board->isDeadPosition());

    expectedBoardState = ".......k-\n.......\n........\n........\n........\n........\n........\n.B-.....K-\n";
    board->initializeBoardFromString(expectedBoardState);
    EXPECT_TRUE(board->isDeadPosition());

    expectedBoardState = ".......k-\n.......\n........\n........\n........\n........\n......P-.\n.B-.....K-\n";
    board->initializeBoardFromString(expectedBoardState);
    EXPECT_FALSE(board->isDeadPosition());
}

TEST_F(ChessBoardTests, IsPositionOnBoard) {
    EXPECT_TRUE(board->isPositionOnBoard(Position{0, 0}));
    EXPECT_FALSE(board->isPositionOnBoard(Position{8, 8}));
}

TEST_F(ChessBoardTests, IsPawnEligibleForEnPassant) {
    Piece* piece = board->getPieceAtPosition(Position{1, 0});
    Pawn* pawn = dynamic_cast<Pawn*>(piece);
    EXPECT_TRUE(pawn->getEnPassant());
    EXPECT_EQ(pawn->getHasMoved(), 0);
    board->movePiece(Position{1, 0}, Position{3, 0});
    EXPECT_TRUE(pawn->getEnPassant());
    EXPECT_EQ(pawn->getHasMoved(), 1);

    EXPECT_TRUE(board->isPawnEligibleForEnPassant(Position{3, 0}, Colour::BLACK));

    board->movePiece(Position{3, 0}, Position{4, 0});
    EXPECT_FALSE(pawn->getEnPassant());
    EXPECT_FALSE(board->isPawnEligibleForEnPassant(Position{1, 1}, Colour::BLACK));
}

TEST_F(ChessBoardTests, ClearBoard) {
    board->clearBoard();
    std::string boardState = board->getBoardStateAsString();
    std::string expectedBoardState = "........\n........\n........\n........\n........\n........\n........\n........\n";
    EXPECT_EQ(boardState, expectedBoardState);
}

TEST_F(ChessBoardTests, PlacePiece) {
    Position pos(0, 0);
    auto rook = std::make_unique<Rook>(Colour::WHITE, pos, PieceType::ROOK);
    EXPECT_TRUE(board->placePiece(pos, std::move(rook)));

    auto* placedRook = board->getPieceAtPosition(pos);
    EXPECT_NE(placedRook, nullptr);
    EXPECT_EQ(placedRook->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(placedRook->getColour(), Colour::WHITE);
}

TEST_F(ChessBoardTests, InitializeBoardFromString) {
    std::string boardState = "r-.b-q-k-b-n-r-\n.p-p-p-p-p-p-p-\nn-.......\np-.......\nP-.......\n........\n.P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    board->initializeBoardFromString(boardState);
    std::string newBoardState = board->getBoardStateAsString();
    EXPECT_EQ(boardState, newBoardState);

    boardState = "........\n........\n........\n........\n........\n........\n........\n........\n";
    board->initializeBoardFromString(boardState);
    newBoardState = board->getBoardStateAsString();
    EXPECT_EQ(boardState, newBoardState);
}