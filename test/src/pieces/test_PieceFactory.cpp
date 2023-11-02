/**
 * @file test_PieceFactory.cpp
 * @brief Unit tests for the PieceFactory class.
 *
 * This file contains unit tests for the PieceFactory class, including creating a pawn, rook, knight, bishop, queen, king, and invalid piece.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>

#include "PieceFactory.h"

/**
 * @brief Test fixture for PieceFactory tests.
 */
class PieceFactoryTests : public ::testing::Test {
protected:
    PieceFactory* factory;

    // Set up PieceFactory 
    void SetUp() override {
        factory = new PieceFactory();
    }

    // Delete PieceFactory 
    void TearDown() override {
        delete factory;
    }
};

TEST_F(PieceFactoryTests, CreatePawn) {
    auto piece = factory->createPiece(Colour::WHITE, Position(1, 2), PieceType::PAWN);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::WHITE);
    EXPECT_EQ(piece->getPosition(), Position(1, 2));
    EXPECT_EQ(piece->getPieceType(), PieceType::PAWN);
}

TEST_F(PieceFactoryTests, CreateRook) {
    auto piece = factory->createPiece(Colour::BLACK, Position(4, 7), PieceType::ROOK);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::BLACK);
    EXPECT_EQ(piece->getPosition(), Position(4, 7));
    EXPECT_EQ(piece->getPieceType(), PieceType::ROOK);
}

TEST_F(PieceFactoryTests, CreateKnight) {
    auto piece = factory->createPiece(Colour::WHITE, Position(2, 3), PieceType::KNIGHT);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::WHITE);
    EXPECT_EQ(piece->getPosition(), Position(2, 3));
    EXPECT_EQ(piece->getPieceType(), PieceType::KNIGHT);
}

TEST_F(PieceFactoryTests, CreateBishop) {
    auto piece = factory->createPiece(Colour::BLACK, Position(5, 4), PieceType::BISHOP);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::BLACK);
    EXPECT_EQ(piece->getPosition(), Position(5, 4));
    EXPECT_EQ(piece->getPieceType(), PieceType::BISHOP);
}

TEST_F(PieceFactoryTests, CreateQueen) {
    auto piece = factory->createPiece(Colour::WHITE, Position(3, 6), PieceType::QUEEN);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::WHITE);
    EXPECT_EQ(piece->getPosition(), Position(3, 6));
    EXPECT_EQ(piece->getPieceType(), PieceType::QUEEN);
}

TEST_F(PieceFactoryTests, CreateKing) {
    auto piece = factory->createPiece(Colour::BLACK, Position(7, 0), PieceType::KING);

    EXPECT_NE(piece, nullptr);
    EXPECT_EQ(piece->getColour(), Colour::BLACK);
    EXPECT_EQ(piece->getPosition(), Position(7, 0));
    EXPECT_EQ(piece->getPieceType(), PieceType::KING);
}

TEST_F(PieceFactoryTests, CreateInvalidPiece) {
    auto piece = factory->createPiece(Colour::WHITE, Position(0, 0), PieceType::NONE);

    EXPECT_EQ(piece, nullptr);
}