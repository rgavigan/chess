/**
 * @file test_Piece.cpp
 * @brief Unit tests for the Piece class.
 *
 * This file contains unit tests for the Piece class, including the constructor, getColour, getPosition, getColourModifier, isValidMove, and getPieceType methods.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>

#include "Piece.h" 

class MockPiece : public Piece {
public:
    MockPiece(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}
    void updateValidMoves(const ChessBoard& board) override {}
};

/**
 * @brief Test fixture for Piece tests
 */
class PieceTests : public ::testing::Test {
protected:
    Position* cornerPos;
    Position* otherPos;
    MockPiece* whiteCornerPiece;
    MockPiece* whiteTwoTwoPiece;
    MockPiece* blackTwoTwoPiece;

    // Set up mock pieces 
    void SetUp() override {
        cornerPos = new Position(0, 0);
        otherPos = new Position(2, 2);
        whiteCornerPiece = new MockPiece(Colour::WHITE, *cornerPos, PieceType::NONE);
        whiteTwoTwoPiece = new MockPiece(Colour::WHITE, *otherPos, PieceType::NONE);
        blackTwoTwoPiece = new MockPiece(Colour::BLACK, *otherPos, PieceType::NONE);
    }

    // Delete mock pieces 
    void TearDown() override {
        delete cornerPos;
        delete otherPos;
        delete whiteCornerPiece;
        delete whiteTwoTwoPiece;
        delete blackTwoTwoPiece;
    }
};

/**
 * Test Name: Constructor
 * Description: Tests the constructor of the Piece class.
 */
TEST_F(PieceTests, Constructor) {
    EXPECT_EQ(whiteCornerPiece->getColour(), Colour::WHITE);
    EXPECT_EQ(whiteCornerPiece->getPosition(), *cornerPos);
}

/**
 * Test Name: GetColour
 * Description: Tests the getColour method of the Piece class.
 */
TEST_F(PieceTests, GetColour) {
    EXPECT_EQ(blackTwoTwoPiece->getColour(), Colour::BLACK);
}

/**
 * Test Name: GetPosition
 * Description: Tests the getPosition method of the Piece class.
 */
TEST_F(PieceTests, GetPosition) {
    EXPECT_EQ(whiteTwoTwoPiece->getPosition(), *otherPos);
}

/**
 * Test Name: GetColourModifier
 * Description: Tests the getColourModifier method of the Piece class.
 */
TEST_F(PieceTests, GetColourModifier) {
    EXPECT_EQ(whiteTwoTwoPiece->getColourModifier(), -1);
    EXPECT_EQ(blackTwoTwoPiece->getColourModifier(), 1);
}

/**
 * Test Name: IsValidMove
 * Description: Tests the isValidMove method of the Piece class.
 */
TEST_F(PieceTests, IsValidMove) {
    EXPECT_FALSE(whiteCornerPiece->isValidMove(Position{0,0}, *cornerPos)); // Assuming no valid moves are set. 
}

/**
 * Test Name: GetPieceType
 * Description: Tests the getPieceType method of the Piece class.
 */
TEST_F(PieceTests, GetPieceType) {
    EXPECT_EQ(whiteCornerPiece->getPieceType(), PieceType::NONE);
}