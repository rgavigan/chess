/**
 * @file King.h
 * @brief Declaration of the King class.
 * 
 * The King class represents a king piece in a chess game. Derived from the Piece class, it overrides methods to provide 
 * king-specific logic, including movement, capturing, and castling (the mention of "en passant" in the original comment for king 
 * might be an oversight as kings don't have such a feature in standard chess rules).
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef KING_H 
#define KING_H

#include "Piece.h"

class King : public Piece {
public:
    /**
     * @brief Initializes a King object with the specified color, position, and type.
     */
    King(Colour colour, Position position, PieceType type);

    /**
     * @brief Virtual destructor to allow for polymorphic destruction.
     */
    virtual ~King();

    /**
     * @brief Updates the validMoves vector with the current valid moves for the king.
     */
    void updateValidMoves(const ChessBoard& board);

    /**
     * @brief Checks if a King is eligible for castling, adding to validMoves if so.
     */
    void checkAndAddCastlingMove(const ChessBoard& board);

    /** Checks if a King can castle with the Rook at the specified position. */
    bool canCastleWith(const Position& rookPos, const ChessBoard& board) const;

    /** Retrieves the position the King would castle to provided the Rook's position. */
    Position getCastlingPosition(const Position& KingPos) const;

};

#endif // KING_H
