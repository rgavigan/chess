/**
 * @file PieceFactory.h
 * @brief Declaration of the PieceFactory class.
 * 
 * This file contains the declaration of the PieceFactory class. The class provides a factory method
 * to create chess pieces based on type, colour, and position.
 *
 * @author Bradley McGlynn
 * @date October 6th, 2023
 */
#ifndef PIECE_FACTORY_H
#define PIECE_FACTORY_H

#include <memory>
#include "Utilities.h"
#include "Piece.h"

class PieceFactory {
public:
    /** Static factory method to create chess pieces based on type, colour, and position. */
    static std::unique_ptr<Piece> createPiece(Colour colour, Position position, PieceType type);
};

#endif // PIECE_FACTORY_H
