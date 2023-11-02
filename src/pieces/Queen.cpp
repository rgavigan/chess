/**
 * @file Queen.cpp
 * @brief Implementation of the Queen class.
 * 
 * This file contains the implementation of the methods declared in Queen.h.
 * The Queen class is derived from the Piece class and represents a queen piece in a chess game.
 * It overrides the methods to provide queen-specific logic for movement and capturing.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */

#include "Queen.h" 
#include "ChessBoard.h"

/**
 * @brief Constructs a Queen object with the specified color and position.
 * @param colour Color of the queen, either WHITE or BLACK.
 * @param position Initial position of the queen on the chessboard.
 * @param type Type of the piece, in this case QUEEN.
 */
Queen::Queen(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/**
 * @brief Destructor for the Queen class. 
 * @details It cleans up any resources allocated specifically for Queen objects.
 */
Queen::~Queen() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the queen.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @details This method overrides the Piece::updateValidMoves() method to provide queen-specific logic for movement and capturing.
 * The queen can move any number of spaces in a straight line either horizontally, vertically, or diagonally, provided there are no pieces in the way.
 * The queen can capture any enemy piece that is in its path of movement.
 * The queen cannot move to a space that is occupied by a friendly piece.
 * As the queen slides along its path, its movement is stopped upon encountering an enemy piece (after capturing it) or a friendly piece.
 * @warning This method does not check if moving the queen would expose the king to a check. This is handled in the GameController.
 * @see Piece::updateValidMoves()
 */
void Queen::updateValidMoves(const ChessBoard& board) {
    // colour modifier not needed for queen as all movement paths are tested 

    /**
     * @brief Illustrates the movement pattern for a Queen.
     * 
     * - x = queen
     * - o = potential moves
     * 
     *   \verbatim
     *     o     o     o
     *       o   o   o
     *         o o o 
     *     o o o x o o o
     *         o o o 
     *       o   o   o
     *     o     o     o
     *   \endverbatim
     */


    /** Since queens can move and capture on the same movement path,
      * we simply need to find the potential areas it can slide towards 
      * orthogonally and diagonally, provided they don't have a friendly piece present
      * Logic considering if a move will place a player in check is handled in GameController
      */
    
    // Clear the existing valid moves 
    validMoves.clear();

    // Update valid orthogonal moves 
    updateValidOrthogonalMoves(board);

    // Update valid diagonal moves 
    updateValidDiagonalMoves(board);
}

/**
 * @brief Update valid orthogonal moves for the queen.
 * The method will check all four possible diagonal directions and update the list of valid moves.
 * @param board ChessBoard object containing positions of other pieces.
 */
void Queen::updateValidOrthogonalMoves(const ChessBoard& board) {
    int curr_row = getPosition().row;
    int curr_col = getPosition().col;

    // Define the four possible orthogonal directions for a queen 
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int i = 0; i < 4; i++) { // Loop over the four directions 
        int step = 1;
        while (true) { // Loop to move along the direction 
            int temp_row = curr_row + step * directions[i][0];
            int temp_col = curr_col + step * directions[i][1];

            Position temp_position{temp_row, temp_col};

            // Check if the position is inside the board 
            if (!board.isPositionOnBoard(temp_position)) break;

            // Check if the space is friendly or enemy 
            if (board.isSpaceFriendly(temp_position, getColour())) break;

            // Add the valid move to the vector 
            validMoves.emplace_back(temp_position);

            // If the space has an enemy piece, the queen can't move past it 
            if (board.isSpaceEnemy(temp_position, getColour())) break;

            step++;
        }
    }
}

/**
 * @brief Update valid diagonal moves for the queen.
 * The method will check all four possible diagonal directions and update the list of valid moves.
 * @param board ChessBoard object containing positions of other pieces.
 */
void Queen::updateValidDiagonalMoves(const ChessBoard& board) {
    int curr_row = getPosition().row;
    int curr_col = getPosition().col;

    // Define the four possible diagonal directions for a queen 
    int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (int i = 0; i < 4; i++) { // Loop over the four directions 
        int step = 1;
        while (true) { // Loop to move along the direction 
            int temp_row = curr_row + step * directions[i][0];
            int temp_col = curr_col + step * directions[i][1];

            Position temp_position{temp_row, temp_col};

            // Check if the position is inside the board 
            if (!board.isPositionOnBoard(temp_position)) break;

            // Check if the space is friendly or enemy 
            if (board.isSpaceFriendly(temp_position, getColour())) break;

            // Add the valid move to the vector 
            validMoves.emplace_back(temp_position);

            // If the space has an enemy piece, the queen can't move past it 
            if (board.isSpaceEnemy(temp_position, getColour())) break;

            step++;
        }
    }
}
