/**
 * @file King.cpp
 * @brief Implementation of the King class.
 * 
 * This file contains the implementation of the methods declared in King.h.
 * The King class is derived from the Piece class and represents a king piece in a chess game.
 * It overrides the methods to provide king-specific logic for movement and capturing.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */

#include "King.h" 
#include "ChessBoard.h"

/**
 * @brief Constructs a King object with the specified color and position.
 * @param colour Color of the king, either WHITE or BLACK.
 * @param position Initial position of the king on the chessboard.
 * @param type Type of the piece, in this case KING.
 */
King::King(Colour colour, Position position, PieceType type) : Piece(colour, position, type) {}

/**
 * @brief Destructor for the King class. 
 * @details Cleans up any resources allocated specifically for King objects.
 */
King::~King() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the king.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @details This method overrides the Piece::updateValidMoves() method to provide king-specific logic for movement and capturing.
 * The king can move one space in any direction, provided there are no pieces in the way.
 * The king can capture any enemy piece that is in one of the eight spaces adjacent to its current position.
 * The king cannot move to a space that is occupied by a friendly piece.
 * The king cannot move to a space that would place it in check.
 * Additionally, the king can perform a special move known as castling under certain conditions, allowing it to move more than once space.
 * @warning This method does not check if the king is in check after a move is made. This is handled in GameController.
 * @see Piece::updateValidMoves()
 */
void King::updateValidMoves(const ChessBoard& board) {
    // Clear the existing valid moves 
    validMoves.clear();

    int curr_row = getPosition().row;
    int curr_col = getPosition().col;
    // colour modifier not needed for king as all movement paths are tested 

    /**
     * @brief Illustrates the movement pattern for a King.
     * 
     * - x = king
     * - o = potential moves
     * 
     *   \verbatim
     *     o o o 
     *     o x o 
     *     o o o 
     *   \endverbatim
     */


    /** Since kings can move and capture on the same movement path,
      * we simply need to find the potential areas it can slide towards 
      * orthogonally and diagonally, provided they don't have a friendly piece present
      * Logic considering if a move will place a player in check is handled in GameController
      */

    // Define the four possible orthogonal directions for a king 
    int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    
    for (int i = 0; i < 8; i++) { // Loop over the eight directions 
        int temp_row = curr_row + directions[i][0];
        int temp_col = curr_col + directions[i][1];

        Position temp_position{temp_row, temp_col};
        
        // Check if the position is inside the board 
        if (!board.isPositionOnBoard(temp_position)) continue;
        
        // Check if the space is friendly, skip it if so 
        if (board.isSpaceFriendly(temp_position, getColour())) continue;

        // Check if the space is safe to move to for the King, avoiding check 
        if (!board.isSquareSafe(temp_position, getColour())) continue;
        
        validMoves.emplace_back(temp_position);
    }
    // Check and add castling move if possible 
    checkAndAddCastlingMove(board);
}

/**
 * @brief Checks if the King can castle with a rook on a either side and adds the castling move if possible.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 */
void King::checkAndAddCastlingMove(const ChessBoard& board) {
    // Check the conditions for castling 
    if (getHasMoved()) return; // King has already moved
    
    // Check both sides for rooks based on King's colour 
    Position leftRookPos = (getColour() == Colour::WHITE) ? Position{position.row, 0} : Position{position.row, 7};
    Position rightRookPos = (getColour() == Colour::WHITE) ? Position{position.row, 7} : Position{position.row, 0};
    
    // Check conditions and add castling move 
    if (canCastleWith(leftRookPos, board)) validMoves.push_back(getCastlingPosition(leftRookPos));
    if (canCastleWith(rightRookPos, board)) validMoves.push_back(getCastlingPosition(rightRookPos));
}

/**
 * @brief Checks if the King can castle with a rook on a given side.
 * @param rookPos The Position of the rook to check for castling.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 * @return true if the King can castle with the rook, false otherwise.
 */
bool King::canCastleWith(const Position& rookPos, const ChessBoard& board) const {
    // Check if there is a rook at rookPos and if it hasn't moved 
    Piece* rook = board.getPieceAtPosition(rookPos);
    // check that the piece at that position is a Rook, that it hasn't moved, and that it isn't null 
    if (!rook || rook->getPieceType() != PieceType::ROOK || rook->getHasMoved()) return false;
    
    // Check if the squares between the king and the rook are empty 
    int direction = (rookPos.col > position.col) ? 1 : -1;
    for (int col = position.col + direction; col != rookPos.col; col += direction) {
        if (!board.isSpaceEmpty(Position{position.row, col})) return false;
    }
    
    // Check if the squares that the king moves through are not under attack 
    if (!board.isSquareSafe(Position{position.row, position.col + direction}, getColour()) ||
        !board.isSquareSafe(Position{position.row, position.col + 2 * direction}, getColour())) return false;
    
    return true;
}

/**
 * @brief Gets the position of the king after castling with a rook on a given side.
 * @param rookPos The Position of the rook to castle with.
 * @return The Position of the king after castling.
 */
Position King::getCastlingPosition(const Position& rookPos) const {
    int direction = (rookPos.col > position.col) ? 1 : -1;
    return Position{position.row, position.col + 2 * direction};
}
