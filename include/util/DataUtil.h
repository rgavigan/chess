/**
 * @file DataUtil.h
 * @brief This header defines enums and structs used across the program.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#ifndef DATAUTIL_H 
#define DATAUTIL_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <regex>


class Player; // forward declaration of the Player class

/**
 * @brief Enumeration representing the colour of a chess piece
 */
enum class Colour { 
    WHITE, 
    BLACK 
};

/**
 * @brief Enumeration representing the type of a chess piece
 */
enum class PieceType {
    PAWN, 
    ROOK, 
    KNIGHT, 
    BISHOP, 
    QUEEN, 
    KING,
    NONE
};

/**
 * @brief Enumeration representing the current game status
 * 
 */
enum class GameStatus {
    ONGOING,
    CHECK,
    CHECKMATE,
    STALEMATE,
    PROMPTDRAW,
    DRAW,
    RESIGN,
    TIMEOUT
};


/**
 * @brief Structure representing a position on the chess board
 * 
 */
struct Position {
    int row; //*< Row of the position */
    int col; //*< Column of the position */

    /**
     * @brief Construct a new Position object
     * 
     * @param r row of the position
     * @param c column of the position
     */
    Position(int r, int c) : row(r), col(c) {}

    /**
     * @brief Equality operator to compare two positions
     * 
     * @param other the other position to compare
     * @return true if the positions are equal
     * @return false if the positions are not equal
     */
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    /**
     * @brief Print the position
     * 
     * @param os output stream
     * @param pos position to print
     * @return std::ostream& output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        os << "(" << pos.row << ", " << pos.col << ")";
        return os;
        
    }
};

/**
 * @brief Structure representing a move in the chess game
 */
struct Move {
    Position start; //* Starting position of the move */
    Position end;   //* Ending position of the move */
    PieceType pieceMoved; //* Type of the piece that was moved */
    PieceType pieceCaptured; //* Type of the piece that was captured, if any */
    Colour playerColour; //* Colour of the player who made the move */
    PieceType promotionPiece; //* Type of the piece that the pawn was promoted to, if any */

    // Default constructor
    Move() 
        : start(Position{0,0}), end(Position{0,0}), pieceMoved(PieceType::NONE), 
          playerColour(Colour::WHITE), pieceCaptured(PieceType::NONE), promotionPiece(PieceType::NONE) {}

    // Constructor to initialize a move with specific values
    Move(Position s, Position e, PieceType pMoved, Colour pl, PieceType pCaptured = PieceType::NONE, PieceType promotionPiece = PieceType::NONE)
        : start(s), end(e), pieceMoved(pMoved), playerColour(pl), pieceCaptured(pCaptured), promotionPiece(promotionPiece) {}
};

/**
 * @brief Structure representing the metadata of the board after a move is made.
*/ 
struct BoardMetadata {
    Player* currentPlayer;         //* Pointer to the current player */
    Player* opponentPlayer;        //* Pointer to the opponent player */
    Colour currentPlayerColour;    //* Colour of the current player */
    Colour opponentPlayerColour;   //* Colour of the opponent player */
    std::string boardState;        //* Board state after the move */
    int turnNumber;                //* The turn number */
    GameStatus gameStatus;         //* The status of the game */
    Move moveMade;                 //* The exact move made */
    std::string PGNstring;         //* PGN notation of the move */
    std::string timestamp;         //* Timestamp of when the move was made */

    // Constructor to initialize all the fields
    BoardMetadata(Player* currentPlayer, Player* opponentPlayer, Colour currentPlayerColour, Colour opponentPlayerColour, 
                  const std::string& boardState, int turnNumber, GameStatus gameStatus, const Move& moveMade,
                  const std::string& PGNstring, const std::string& timestamp)
        : currentPlayer(currentPlayer), opponentPlayer(opponentPlayer), currentPlayerColour(currentPlayerColour), opponentPlayerColour(opponentPlayerColour),
          boardState(boardState), turnNumber(turnNumber), gameStatus(gameStatus), moveMade(moveMade),
          PGNstring(PGNstring), timestamp(timestamp) {}
};

#endif // DATAUTIL_H