/**
 * @file Knight.h
 * @brief Declaration of the Knight class.
 * 
 * The Knight class represents a knight piece in a chess game. Derived from the Piece class, it overrides methods to provide 
 * knight-specific logic, including movement and capturing. Note: The mention of "en passant" eligibility for the knight in the 
 * original comment might be an oversight, as knights do not have such a feature in standard chess rules.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef KNIGHT_H 
#define KNIGHT_H

#include "Piece.h"

class Knight : public Piece {
public:
    /**
     * @brief Initializes a Knight object with the specified color, position, and type.
     */
    Knight(Colour colour, Position position, PieceType type);

    /**
     * @brief Virtual destructor to allow for polymorphic destruction.
     */
    virtual ~Knight();

    /**
     * @brief Updates the validMoves vector with the current valid moves for the knight.
     */
    void updateValidMoves(const ChessBoard& board);

};

#endif // KNIGHT_H
