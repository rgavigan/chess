/**
 * @file Pawn.h
 * @brief Declaration of the Pawn class.
 * 
 * The Pawn class represents a pawn piece in a chess game. Derived from the Piece class, it overrides methods to provide pawn-specific 
 * logic, including movement, capturing, and en passant eligibility.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef PAWN_H 
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece {
public:
    /**
     * @brief Initializes a Pawn object with the specified color, position, and type.
     */
    Pawn(Colour colour, Position position, PieceType type);

    /**
     * @brief Virtual destructor to allow for polymorphic destruction.
     */
    virtual ~Pawn();

    /**
     * @brief Updates the validMoves vector with the current valid moves for the pawn.
     */
    void updateValidMoves(const ChessBoard& board);

    /**
     * @brief Sets the enPassant eligibility of the pawn.
     */
    void setEnPassant();

    /**
     * @brief Retrieves the enPassant eligibility of the pawn.
     * @return true if eligible for en passant, false otherwise.
     */
    bool getEnPassant() const;

    /**
     * @brief Retrieves the move counter of the pawn.
     * @return Number of moves made by the pawn.
     */
    int getMoveCounter() const;

    /**
     * @brief Increments the move counter of the pawn.
     */
    void incrementMoveCounter();

private:
    bool enPassant = true; /** Flag indicating if the pawn is eligible for en passant. */
    int moveCounter = 0;   /** Counter tracking moves the pawn has made, for tracking en passant eligibility. */
};

#endif // PAWN_H
