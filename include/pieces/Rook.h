/**
 * @file Rook.h
 * @brief Declaration of the Rook class.
 * 
 * This file contains the declaration of the Rook class, which represents a rook piece in a chess game.
 * The Rook class is derived from the Piece class and overrides its methods to provide rook-specific logic 
 * for movement, capturing, and en passant eligibility.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef ROOK_H 
#define ROOK_H 

#include "Piece.h" 

class Rook : public Piece {
public:
    /** Initializes a Rook object with the specified color, position, and type. */
    Rook(Colour colour, Position position, PieceType type);

    /**
     * @brief Destructor: Cleans up any resources allocated specifically for Rook objects.
     */
    virtual ~Rook();

    /**
     * @brief Method to update the validMoves vector with the current valid moves for the Rook.
     * @param board Reference to the chess board to calculate valid moves.
     */
    void updateValidMoves(const ChessBoard& board);
};

#endif // ROOK_H
