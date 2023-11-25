/**
 * @file ChessBoard.h
 * @brief Declaration of the ChessBoard class.
 * 
 * The ChessBoard class represents the game's chessboard and contains methods to interact with it, such as moving pieces, checking move validity,
 * getting pieces by position, and more.
 *
 * @author Bradley McGlynn
 * @date September 24th, 2023
 */

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <vector>
#include <memory>
#include "DataUtil.h"

class Piece;

class ChessBoard {
public:
    /** Initializes the board to a default state. */
    ChessBoard(); /** @brief Constructor to initialize the board. */
    
    /** Destructor that ensures proper memory cleanup. */
    ~ChessBoard(); 

    /** Initializes all the pieces on the board at the start of a match. */
    void initializeBoard(bool testMode = false);

    /** Get the piece at a given position. */
    Piece* getPieceAtPosition(const Position& position) const;
    
    /** Move a piece from start to end position. */
    bool movePiece(const Position& start, const Position& end, bool override = false);

    /** Checks if a move from start to end position is valid. */
    bool isValidMove(const Position& start, const Position& end) const;    
    
    /** Check if a space on the board is empty. */
    bool isSpaceEmpty(const Position& position) const;

    /** Check if a space contains a friendly piece. */
    bool isSpaceFriendly(const Position& position, Colour colour) const;

    /** Check if a space contains an opponent's piece. */
    bool isSpaceEnemy(const Position& position, Colour colour) const;

    /** Check if a square is safe from enemy attacks, particularly for the King. */
    bool isSquareSafe(const Position& pos, Colour colour) const;

    /** Returns the position of the king for the player with the specified colour. */
    Position getKingPosition(Colour colour) const;

    /** Returns a vector of pointers to Piece objects for all pieces of a specified player colour. */
    std::vector<Piece*> getPiecesOfColour(Colour colour) const;

    /** Returns a string representation of the current board state. */
    std::string getBoardStateAsString() const;

    /** Checks if the board is in a dead position (no legal moves left). */
    bool isDeadPosition() const;

    /** Checks if a given position is a valid position on the board. */
    bool isPositionOnBoard(const Position& position) const;

    /** Checks if a pawn at a given position is eligible to be captured en passant. */
    bool isPawnEligibleForEnPassant(const Position& position, Colour colour) const;

    /** Removes all pieces from the board. */
    void clearBoard();

    /** Places a piece on the board at a given position. */
    bool placePiece(const Position& position, std::unique_ptr<Piece> piece);

    /** Initializes the board state from a given string representation. */
    void initializeBoardFromString(const std::string& boardState);

    /** Overloaded operator[] to access a specific row on the board. */
    std::vector<std::unique_ptr<Piece>>& operator[](int row);

    /** Overloaded operator[] with const-qualifier to access a specific row on the board without modification. */
    const std::vector<std::unique_ptr<Piece>>& operator[](int row) const;
private:
    std::vector<std::vector<std::unique_ptr<Piece>>> board; /**< @brief 2D vector representing the chessboard. Each inner vector represents a row, and each unique pointer within represents a square that may contain a piece. */
};

#endif // CHESS_BOARD_H
