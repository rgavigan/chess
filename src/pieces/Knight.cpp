/**
 * @file Knight.cpp
 * @author Brad McGlynn
 * @date September 23rd, 2023
 * @brief Implementation of the Knight class.
 * @details This file contains the implementation of the methods declared in Knight.h. 
 * The Knight class is derived from the Piece class and represents a knight piece in a chess game. 
 * It overrides the methods to provide knight-specific logic for movement and capturing.
 */

#include "Knight.h" 

/**
 * @brief Constructs a Knight object with the specified color and position.
 * @param colour Color of the knight, either WHITE or BLACK.
 * @param position Initial position of the knight on the chessboard.
 * @param type Type of the piece, in this case KNIGHT.
 */
Knight::Knight(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/**
 * @brief Destructor for the Knight class. 
 * @details It cleans up any resources allocated specifically for Knight objects.
 */
Knight::~Knight() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the knight.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @details This method overrides the Piece::updateValidMoves() method to provide knight-specific logic for movement and capturing.
 * Knights move in an L-shape pattern: they can move two squares in one dimension and one square in the other. 
 * They can jump over other pieces.
 * Knights can capture any enemy piece that lies at the destination of their L-shaped movement.
 * Knights cannot land on a square occupied by a friendly piece.
 * Unlike most other pieces, knights are not blocked by other pieces on the board.
 * @warning This method does not check if the king is in check after a move is made. This is handled in GameController.
 * @see Piece::updateValidMoves()
 */
void Knight::updateValidMoves(const ChessBoard& board) {
    // Clear the existing valid moves 
    validMoves.clear();

    int curr_row = getPosition().row;
    int curr_col = getPosition().col;
    int colour_modifier = getColourModifier();

    /**
     * @brief Illustrates the movement pattern for a Knight.
     * 
     * - x = knight
     * - o = potential moves
     * - - = spaces jumped over
     * 
     *   \verbatim
     *         1   2
     *         o - o
     *     3 o - - - o 4
     *           x
     *     5 o - - - o 6
     *         o - o
     *         7   8
     *   \endverbatim
     */


    // Define possible moves for a knight in terms of row and column offsets 
    int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

    for (int i = 0; i < 8; i++) {
        int temp_row = curr_row + moves[i][0] * colour_modifier;
        int temp_col = curr_col + moves[i][1] * colour_modifier;

        Position temp_position{temp_row, temp_col};

        // Check if the position is inside the board 
        if (!board.isPositionOnBoard(temp_position)) continue;

        // Check if the space is friendly, skip it if so 
        if (board.isSpaceFriendly(temp_position, getColour())) continue;

        validMoves.emplace_back(temp_position);
    }
}
