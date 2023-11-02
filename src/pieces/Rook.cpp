/**
 * @file Rook.cpp
 * @brief Implementation of the Rook class.
 * 
 * This file contains the implementation of the methods declared in Rook.h.
 * The Rook class is derived from the Piece class and represents a rook piece in a chess game.
 * It overrides the methods to provide rook-specific logic for movement and capturing.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */

#include "Rook.h" 
#include "ChessBoard.h"

/**
 * @brief Constructs a Rook object with the specified color and position.
 * @param colour Color of the rook, either WHITE or BLACK.
 * @param position Initial position of the rook on the chessboard.
 * @param type Type of the piece, in this case ROOK.
 */
Rook::Rook(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/**
 * @brief Destructor for the Rook class. 
 * @details Cleans up any resources allocated specifically for Rook objects.
 */
Rook::~Rook() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the rook.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @details This method overrides the Piece::updateValidMoves() method to provide rook-specific logic for movement and capturing.
 * The rook can move any number of spaces in a straight line either horizontally or vertically, provided there are no pieces in the way.
 * The rook can capture any enemy piece that is in its path of movement.
 * The rook cannot move to a space that is occupied by a friendly piece.
 * As the rook slides along its path, its movement is stopped upon encountering an enemy piece (after capturing it) or a friendly piece.
 * @warning This method does not check if moving the rook would expose the rook to a check. This is handled in the GameController.
 * @see Piece::updateValidMoves()
 */
void Rook::updateValidMoves(const ChessBoard& board) {
    // Clear the existing valid moves 
    validMoves.clear();

    int curr_row = getPosition().row;
    int curr_col = getPosition().col;
    // colour modifier not needed for rook as all horizontal and vertical paths are tested 

    /**
     * @brief Illustrates the movement pattern for a Rook.
     * 
     * - x = rook
     * - o = potential moves
     * 
     *   \verbatim
     *           o
     *           o
     *           o
     *     o o o x o o o
     *           o
     *           o
     *           o
     *   \endverbatim
     */


    /** Since rooks can move and capture on the same movement path,
      * we simply need to find the potential areas it can slide towards 
      * horizontally and vertically, provided they don't have a friendly piece present
      * Logic considering if a move will place a player in check is handled in GameController
      */

    // Define the four possible orthogonal directions for a rook 
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int i = 0; i < 4; i++) { // Loop over the four diagonal directions 
        int step = 1;
        while (true) { // Loop to move along the diagonal 
            int temp_row = curr_row + step * directions[i][0];
            int temp_col = curr_col + step * directions[i][1];

            Position temp_position{temp_row, temp_col};
            
            // Check if the position is inside the board 
            if (!board.isPositionOnBoard(temp_position)) break;
            
            // Check if the space is friendly or enemy 
            if (board.isSpaceFriendly(temp_position, getColour())) break;
            
            validMoves.emplace_back(temp_position);
            
            // If the space has an enemy piece, the rook can't move past it 
            if (board.isSpaceEnemy(temp_position, getColour())) break;
            
            step++;
        }
    }

}
