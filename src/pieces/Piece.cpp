/**
 * @file Piece.cpp
 * @brief Implementation of the Piece class.
 * 
 * This file contains the implementation of the methods declared in Piece.h.
 * The Piece class serves as an abstract base class for different types of chess pieces.
 * It contains common attributes and methods that all derived piece classes will inherit and override.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */

#include "Piece.h"
#include "ChessBoard.h"

/**
 * @brief Constructs a Piece object with the specified colour, position, and type.
 * @param colour Colour of the piece, either WHITE or BLACK.
 * @param position Initial position of the piece on the chessboard.
 * @param type Type of the piece, in this case NONE.
 */
Piece::Piece(Colour colour, Position position, PieceType type) 
    : colour(colour), position(position), pieceType(type), hasMoved(false) {}

/**
 * @brief Virtual destructor for the Piece class.
 */
Piece::~Piece() {}

/**
 * @brief Updates the validMoves vector with the current valid moves for the piece.
 * @param board The ChessBoard object used to validate moves and check potential spaces for moves.
 */
void Piece::updateValidMoves(const ChessBoard& board) {}

/**
 * @brief Returns the colour of the Piece.
 * @return Colour of the piece.
 */
Colour Piece::getColour() const {
    return this->colour;
}

/**
 * @brief Returns the position of the piece.
 * @return Position of the piece.
 */
Position Piece::getPosition() const {
    return this->position;
}

/**
 * @brief Returns the vector of valid moves for the piece.
 * @return Vector of valid moves.
 */
std::vector<Position> Piece::getValidMoves() const {
    return this->validMoves;
}

/**
 * @brief Sets the position of the piece.
 * @param position Position to set the piece to.
 */
void Piece::setPosition(const Position& position) {
    this->position = position;
}

/**
 * @brief Returns the colour modifier for piece movement logic based on the piece's colour.
 * @return Colour modifier as an integer.
 */
int Piece::getColourModifier() const {
    return colour == Colour::WHITE ? -1 : 1;
}

/**
 * @brief Checks if the piece made a valid move based on start and end position.
 * Compares the start and end position of the piece in comparison to its vector of valid moves.
 * @param start Start position.
 * @param end End position.
 * @return True if the move is valid, otherwise false.
 */
bool Piece::isValidMove(const Position& start, const Position& end) {
    for(const auto& move : validMoves) {
        if(move == end) return true;
    }
    return false;
}

/**
 * @brief Returns the type of the piece.
 * @return Piece type.
 */
PieceType Piece::getPieceType() const {
    return this->pieceType;
}

/**
 * @brief Sets hasMoved to true after a piece has made its first move.
 * Relevant for pawn en passant + king and rook castling.
 */
void Piece::setHasMoved() {
    this->hasMoved = true;
}

/**
 * @brief Checks if the piece has made its first move.
 * @return True if the piece has moved, otherwise false.
 */
bool Piece::getHasMoved() const {
    return this->hasMoved;
}
