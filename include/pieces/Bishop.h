/**
 * @file Bishop.h
 * @brief Declaration of the Bishop class.
 * 
 * The Bishop class represents a bishop piece in a chess game. Derived from the Piece class, it overrides methods to provide 
 * bishop-specific logic for movement and capturing. (The mention of "en passant" in the original comment for bishop is likely 
 * an oversight since bishops don't have such a feature in standard chess rules).
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef BISHOP_H 
#define BISHOP_H

#include "Piece.h"

class Bishop : public Piece {
public:
    /**
     * @brief Initializes a Bishop object with the specified color, position, and type.
     */
    Bishop(Colour colour, Position position, PieceType type);

    /**
     * @brief Virtual destructor to allow for polymorphic destruction.
     */
    virtual ~Bishop();

    /** Updates the validMoves vector with the current valid moves for the bishop. */
    void updateValidMoves(const ChessBoard& board);

};

#endif // BISHOP_H
