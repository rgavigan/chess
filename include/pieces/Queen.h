/**
 * @file Queen.h
 * @brief Declaration of the Queen class.
 * 
 * This file contains the declaration of the Queen class, which represents a queen piece in a chess game.
 * The Queen class is derived from the Piece class and overrides its methods to provide queen-specific 
 * logic for movement, capturing, and en passant eligibility.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef QUEEN_H 
#define QUEEN_H 

#include "Piece.h"

class Queen : public Piece {
public:
    /** Initializes a Queen object with the specified color, position, and type. */
    Queen(Colour colour, Position position, PieceType type);

    /**
     * @brief Destructor: Cleans up any resources allocated specifically for Queen objects.
     */
    virtual ~Queen();

    /** Method to update the validMoves vector with the current valid moves for the Queen */
    void updateValidMoves(const ChessBoard& board);

    /** Helper method to find the valid orthogonal moves for the Queen. */
    void updateValidOrthogonalMoves(const ChessBoard& board);

    /** Helper method to find the valid diagonal moves for the Queen. */
    void updateValidDiagonalMoves(const ChessBoard& board);
};

#endif // QUEEN_H
