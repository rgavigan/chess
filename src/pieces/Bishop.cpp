/**
 * @brief Implementation of the Bishop class.
 * This file contains the implementation of the methods declared in Bishop.h.
 * The Bishop class is derived from the Piece class and represents a bishop piece in a chess game.
 * It overrides the methods to provide bishop-specific logic for movement and capturing.
 * @date September 23rd, 2023
 * @author Brad McGlynn 
 */
#include "Bishop.h" 
#include "ChessBoard.h"

/**
 * @brief Constructs a Bishop object with the specified color and position.
 * @param colour Color of the bishop, either WHITE or BLACK.
 * @param position Initial position of the bishop on the chessboard.
 * @param type Type of the piece, in this case BISHOP. 
 */ 
Bishop::Bishop(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/**
 * @brief Destructor for the Bishop class. It cleans up any resources allocated specifically for Bishop objects. 
 */ 
Bishop::~Bishop() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the bishop.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves. 
 * @details This method overrides the Piece::updateValidMoves() method to provide bishop-specific logic for movement and capturing.
 * The bishop can move any number of spaces diagonally, provided there are no pieces in the way.
 * The bishop can capture any enemy piece that is on a diagonal path from its current position.
 * The bishop cannot move past any piece, friendly or enemy, that is in its path.
 * The bishop cannot capture any friendly piece that is in its path.
 * The bishop cannot move to a space that would place its own king in check.
 * @warning This method does not check if the king is in check after a move is made. This is handled in GameController.
 * @see Piece::updateValidMoves() 
 */
void Bishop::updateValidMoves(const ChessBoard& board) {
  // Clear the existing valid moves 
  validMoves.clear();

  int curr_row = getPosition().row;
  int curr_col = getPosition().col;
  // colour modifier not needed for bishop as all diagonal paths are tested 

  /**
   * @brief Illustrates the movement pattern for a Bishop.
   * 
   * - x = bishop
   * - o = potential moves
   * 
   *   \verbatim
   *     o           o
   *       o       o
   *         o   o 
   *           x
   *         o   o 
   *       o       o
   *     o           o
   *   \endverbatim
   */


  /** Since bishops can move and capture on the same movement path,
   *  we simply need to find the potential areas it can slide towards diagonally
   * provided they don't have a friendly piece present
   * Logic considering if a move will place a player in check is handled in GameController
   */

  // Define the four possible diagonal directions for a bishop 
  int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

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
          
          // If the space has an enemy piece, the bishop can't move past it 
          if (board.isSpaceEnemy(temp_position, getColour())) break;
          
          step++;
      }
  }
}
