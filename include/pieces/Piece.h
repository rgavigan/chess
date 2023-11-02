/**
 * @file Piece.h
 * @brief Declaration of the Piece class.
 * 
 * The Piece class serves as the base class for all the different types of chess pieces. It provides a common interface for interacting 
 * with chess pieces, such as move validation, position retrieval, and other piece-related functionalities. Each piece has attributes like 
 * colour, position, type, and a list of valid moves. The Piece class also tracks whether a piece has moved during a game. Designed to be 
 * extensible, it allows for easy addition of new piece types and functionalities.
 *
 * @author Brad McGlynn
 * @date September 23rd, 2023
 */
#ifndef PIECE_H 
#define PIECE_H

#include <string>
#include <vector>  // For storing valid moves.
#include "Utilities.h"
#include "ChessBoard.h"

class Piece {
public:
    /**
     * @brief Initializes the piece with a specified colour, position, and type.
     */
    Piece(Colour colour, Position position, PieceType type);
    
    /**
     * @brief Virtual Destructor for polymorphic destruction.
     */
    virtual ~Piece();

    /**
     * @brief Validates if a move is valid for this piece.
     * @return true if the move is valid, false otherwise.
     */
    bool isValidMove(const Position& start, const Position& end);
    
    /**
     * @brief Pure virtual function to update the valid moves for this piece. To be implemented by derived classes.
     */
    virtual void updateValidMoves(const ChessBoard& board) = 0;

    /**
     * @brief Returns the colour modifier for this piece.
     */
    int getColourModifier() const;

    /**
     * @brief Returns the colour of this piece.
     */
    Colour getColour() const;

    /**
     * @brief Returns the position of this piece.
     */
    Position getPosition() const;

    /**
     * @brief Returns the vector of valid moves for this piece.
     */
    std::vector<Position> getValidMoves() const;

    /**
     * @brief Sets the position of this piece.
     */
    void setPosition(const Position& position);

    /**
     * @brief Returns the type of this piece.
     */
    PieceType getPieceType() const;

    /**
     * @brief Sets whether this piece has moved.
     */
    void setHasMoved();

    /**
     * @brief Returns whether this piece has moved.
     */
    bool getHasMoved() const;

protected:
    Colour colour;                    /** The colour of this piece. */
    Position position;                /** The position of this piece on the board. */
    std::vector<Position> validMoves; /** A list of valid moves for this piece. */
    PieceType pieceType;              /** The type of this piece. */
    bool hasMoved;                    /** Whether this piece has moved during the game. */
};

#endif // PIECE_H 
