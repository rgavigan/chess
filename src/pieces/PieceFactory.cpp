/**
 * @file PieceFactory.cpp
 * @brief Implementation of the PieceFactory class.
 * 
 * This file contains the implementation of the methods declared in PieceFactory.h.
 * The PieceFactory class provides a factory method to create various chess pieces.
 *
 * @author Brad McGlynn
 * @date October 6th, 2023
 */

#include "PieceFactory.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"

/**
 * @brief Static factory method to create chess pieces based on type, colour, and position.
 * Depending on the piece type, this method will create and return an appropriate chess piece.
 * @param colour Colour of the piece, either WHITE or BLACK.
 * @param position Position of the piece on the chessboard.
 * @param type Type of the piece.
 * @return std::unique_ptr to the created Piece.
 */
std::unique_ptr<Piece> PieceFactory::createPiece(Colour colour, Position position, PieceType type) {
    switch (type) {
        case PieceType::PAWN:
            return std::make_unique<Pawn>(colour, position, type);
        case PieceType::ROOK:
            return std::make_unique<Rook>(colour, position, type);
        case PieceType::KNIGHT:
            return std::make_unique<Knight>(colour, position, type);
        case PieceType::BISHOP:
            return std::make_unique<Bishop>(colour, position, type);
        case PieceType::QUEEN:
            return std::make_unique<Queen>(colour, position, type);
        case PieceType::KING:
            return std::make_unique<King>(colour, position, type);
        default:
            return nullptr; // Return nullptr for PieceType::NONE or any other unexpected type. 
    }
}
