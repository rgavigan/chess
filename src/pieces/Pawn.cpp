/**
 * @file Pawn.cpp
 * @brief Implementation of the Pawn class.
 * 
 * This file contains the implementation of the methods declared in Pawn.h.
 * The Pawn class is derived from the Piece class and represents a pawn piece in a chess game.
 * It overrides the methods to provide pawn-specific logic for movement and capturing.
 * 
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */

#include "Pawn.h" 
#include "ChessBoard.h"

/**
 * @brief Constructs a Pawn object with the specified color and position.
 * @param colour Color of the pawn, either WHITE or BLACK.
 * @param position Initial position of the pawn on the chessboard.
 * @param type Type of the piece, in this case PAWN.
 */
Pawn::Pawn(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/** 
 * @brief Destructor for the Pawn class.
 * @details It cleans up any resources allocated specifically for Pawn objects.
 */
Pawn::~Pawn() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the pawn.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @details This method takes into consideration the pawn's specific movements including moving forward, capturing diagonally,
 * the double forward move from the starting position, and the special en passant rule.
 * @see Piece::updateValidMoves()
 */
void Pawn::updateValidMoves(const ChessBoard& board) {
    validMoves.clear(); // Clear the existing valid moves 

    int curr_row = getPosition().row;
    int curr_col = getPosition().col;
    int colour_modifier = getColourModifier(); /** -1 for WHITE, 1 for BLACK
    
    /**
     * @brief Illustrates the different movement patterns for a Pawn.
     * 
     * - x = pawn
     * - o = empty space
     * - - = jumped over spot
     * - y = enemy pawn
     * 
     * - Normal / forward:
     *   \verbatim
     *       o
     *       x
     *   \endverbatim
     * 
     * - Forward double move (only from starting position):
     *   \verbatim
     *       o
     *       -
     *       x
     *   \endverbatim
     * 
     * - Diagonal Capture:
     *   \verbatim
     *     o   o
     *       x
     *   \endverbatim
     * 
     * - En Passant (only when the enemy pawn has moved 2 spots in its previous turn i.e. has double moved forward from its starting spot):
     *   \verbatim
     *     o   o
     *     y x y
     *   \endverbatim
     */

    
    // Forward Move 
    int forward_row = curr_row + (1 * colour_modifier);
    if (board.isPositionOnBoard(Position{forward_row, curr_col}) && board.isSpaceEmpty(Position{forward_row, curr_col})) {
        validMoves.push_back(Position{forward_row, curr_col});
        
        // Double Forward Move from Starting Position 
        if ((colour_modifier == 1 && curr_row == 1) || (colour_modifier == -1 && curr_row == 6)) {
            int double_forward_row = curr_row + (2 * colour_modifier);
            if (board.isSpaceEmpty(Position{double_forward_row, curr_col})) {
                validMoves.emplace_back(Position{double_forward_row, curr_col});
            }
        }
    }
    
    // Diagonal Captures 
    int diagonal_cols[2] = {curr_col - 1, curr_col + 1};
    for (int col : diagonal_cols) {
        if (board.isPositionOnBoard(Position{forward_row, col})) {
            Position diagonal_position{forward_row, col};
            if (board.isSpaceEnemy(diagonal_position, getColour())) {
                validMoves.emplace_back(diagonal_position);
            }
        }
    }

    // En Passant, can only occur on row 4 (index 3) for white pawns, and row 5 (index 4) for black pawns 
    if ((colour_modifier == 1 && curr_row == 3) || (colour_modifier == -1 && curr_row == 4)) {
        int adjacent_cols[2] = {curr_col - 1, curr_col + 1};
        for (int col : adjacent_cols) {
            if (board.isPositionOnBoard(Position{curr_row, col})) {
                Position adjacent_position{curr_row, col};
                if (board.isPawnEligibleForEnPassant(adjacent_position, oppositeColour(getColour()))) {
                    validMoves.emplace_back(Position{forward_row, col});
                }
            }
        }
    }
}

/**
 * @brief Returns the enPassant member variable.
 * @return true if the pawn is eligible for en passant, false otherwise.
 */
bool Pawn::getEnPassant() const {
    return enPassant;
}

/**
 * @brief Sets the enPassant member variable based on the pawn's position and move counter.
 * @details If a pawn moves two squares forward from its starting position, it becomes eligible for en passant capture on the next turn.
 */
void Pawn::setEnPassant() {
    if ((getColour() == Colour::WHITE && position.row == 4 && getMoveCounter() == 1) 
    || (getColour() == Colour::BLACK && position.row == 3 && getMoveCounter() == 1)) {
        enPassant = true;
        return;
    }
    enPassant = false;
}

/**
 * @brief Returns the moveCounter member variable.
 * @return int describing how many moves a Pawn has made.
 */
int Pawn::getMoveCounter() const {
    return moveCounter;
}

/**
 * @brief Increments the moveCounter member variable by 1.
 */
void Pawn::incrementMoveCounter() {
    moveCounter++;
}
