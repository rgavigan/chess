#ifndef UTILITIES_H 
#define UTILITIES_H

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
#include <sqlite3.h>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <regex>


class Player; // forward declaration of the Player class

/**
 * Author: Bradley McGlynn
 * Description: Declaration of various utility methods, enums, and structs used throughout the chess game.
 * Date: September 24th, 2023
 */

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
 * @brief Enumeration representing the difficulty of the stockfish AI opponent
 * 
 */
enum class Difficulty {
    HARD,
    MEDIUM,
    EASY
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

// Simple method to get the opposite colour of a queried players.
inline Colour oppositeColour(Colour colour) {
    return (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
}

// Structure representing the metadata of the board after a move is made
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

/**
 * Function Name: splitString
 * Description: Splits a string into a vector of strings based on a delimiter
 * Parameters: string to split and the delimiter to split on
 * Return: vector of strings
 */
inline std::vector<std::string> splitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

/**
 * Function Name: parseStockfishOutput
 * Description: Parses stockfish output into a vector of suggested moves as string 
 * Parameters: string representing the stockfish output.
 * Return: vector of suggested moves as string
 */
inline std::vector<std::string> parseStockfishOutput(const std::string& output) {
    std::vector<std::string> suggestedMoves;

    // Use regular expressions to match lines containing suggested moves, using a-h as columns as the first and third characters and 1-8 as rows as the second and fourth characters
    std::regex movePattern(R"(\b[a-h]\d[a-h]\d\b)");

    // Split the output into lines using the split function
    std::vector<std::string> lines = splitString(output, '\n');

    for (const std::string& line : lines) {
        std::smatch match;
        if (std::regex_search(line, match, movePattern)) {
            for (const auto& m : match) {
                suggestedMoves.push_back(m.str());
            }
        }
    }

    return suggestedMoves;
}

/**
 * Function Name: turnMoveIntoString
 * Description: Turns the player's last move into a readable string for stockfish.
 * Parameters:
 * - move: The last move made in the game as a Move object.
 * Return: A string representing the last move made in the game.
 */
inline std::string turnMoveIntoString(const Move& move) {
    std::string moveString = "";
    std::unordered_map<int, char> columnToChar = {{0, 'a'}, {1, 'b'}, {2, 'c'}, {3, 'd'}, {4, 'e'}, {5, 'f'}, {6, 'g'}, {7, 'h'}};

    std::string startRow = std::to_string(abs(move.start.row - 8));
    std::string endRow =  std::to_string(abs(move.end.row - 8));

    moveString += columnToChar[move.start.col] + startRow + columnToChar[move.end.col] + endRow;

    return moveString;
}

inline std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    std::stringstream ss;
    ss << std::put_time(parts, "%Y-%m-%d");
    return ss.str();
}

inline  std::string positionToChessNotation(const Position& pos) {
    //std::unordered_map<int, char> columnToChar
    char file = 'a' + pos.col;
    int rank = 8 - pos.row;
    return std::string(1, file) + std::to_string(rank);
}

inline std::string pieceTypeToNotation(PieceType piece) {
    switch (piece) {
        case PieceType::PAWN: return "";
        case PieceType::ROOK: return "R";
        case PieceType::KNIGHT: return "N";
        case PieceType::BISHOP: return "B";
        case PieceType::QUEEN: return "Q";
        case PieceType::KING: return "K";
        default: return "";
    }
}

const std::map<std::string, GameStatus> stringToStatusMap = {
    {"ONGOING", GameStatus::ONGOING},
    {"CHECK", GameStatus::CHECK},
    {"PROMPTDRAW", GameStatus::PROMPTDRAW},
    {"CHECKMATE", GameStatus::CHECKMATE},
    {"STALEMATE", GameStatus::STALEMATE},
    {"DRAW", GameStatus::DRAW},
    {"RESIGN", GameStatus::RESIGN}
};

const std::map<GameStatus, std::string> statusToStringMap = {
    {GameStatus::ONGOING, "ONGOING"},
    {GameStatus::CHECK, "CHECK"},
    {GameStatus::PROMPTDRAW, "PROMPTDRAW"},
    {GameStatus::CHECKMATE, "CHECKMATE"},
    {GameStatus::STALEMATE, "STALEMATE"},
    {GameStatus::DRAW, "DRAW"},
    {GameStatus::RESIGN, "RESIGN"}
};

inline GameStatus stringToGameStatus(const std::string& statusStr) {
    auto it = stringToStatusMap.find(statusStr);
    if (it != stringToStatusMap.end()) {
        return it->second;
    }

    std::cerr << "Unknown game status string: " << statusStr << std::endl;
    return GameStatus::ONGOING;
}

inline std::string gameStatusToString(GameStatus status) {
    auto it = statusToStringMap.find(status);
    if (it != statusToStringMap.end()) {
        return it->second;
    }

    std::cerr << "Unknown game status: " << static_cast<int>(status) << std::endl;
    return "UNKNOWN";
}

inline std::string generatePGN(
    const std::string& username1,
    const std::string& whitePlayer,
    const std::string& blackPlayer,
    const GameStatus gameStatus,
    const std::vector<BoardMetadata>& chronologicalData
) {
    std::stringstream pgn;

    std::string eventName = "Local Match: " + whitePlayer + " " + blackPlayer;
    std::string site = "CS 3307";
    std::string date = getCurrentDate(); 
    std::string round = "1";

    std::string matchResult;
        if (gameStatus == GameStatus::CHECKMATE || gameStatus == GameStatus::RESIGN || gameStatus == GameStatus::TIMEOUT) {
            matchResult = username1 == blackPlayer ? "1-0" : "0-1";
        } else if (gameStatus == GameStatus::DRAW || gameStatus == GameStatus::STALEMATE) {
            matchResult = "1/2-1/2";
        } else {
            matchResult = "*";  // Uncertain outcome or game still ongoing
        }

    // Add meta information
    pgn << "[Event \"" << eventName << "\"]\n"
        << "[Site \"" << site << "\"]\n"
        << "[Date \"" << date << "\"]\n"
        << "[Round \"" << round << "\"]\n"
        << "[White \"" << whitePlayer << "\"]\n"
        << "[Black \"" << blackPlayer << "\"]\n"
        << "[Result \"" << matchResult << "\"]\n\n";

    for (const auto& metadata : chronologicalData) {
        const Move& move = metadata.moveMade;
        if (move.start == Position{0,0} && move.end == Position{0,0}) {
            continue;
        }

        // Adding turn number for white moves
        if (move.playerColour == Colour::WHITE) {
            pgn << metadata.turnNumber << ". ";
        }

        // Check for castling
        if (move.pieceMoved == PieceType::KING && abs(move.end.col - move.start.col) == 2) {
            pgn << (move.end.col > move.start.col ? "O-O" : "O-O-O");
        } else {
            pgn << pieceTypeToNotation(move.pieceMoved);

            // For pawns capturing, we include the starting file
            if (move.pieceMoved == PieceType::PAWN && move.pieceCaptured != PieceType::NONE) {
                pgn << static_cast<char>('a' + move.start.col);
            }

            pgn << (move.pieceCaptured != PieceType::NONE ? "x" : "")
                << positionToChessNotation(move.end);

            // Pawn promotion
            if (move.promotionPiece != PieceType::NONE) {
                pgn << "=" << pieceTypeToNotation(move.promotionPiece);
            }
        }

        // Check and Checkmate based on GameStatus from metadata
        if (metadata.gameStatus == GameStatus::CHECKMATE) {
            pgn << "#";
        } else if (metadata.gameStatus == GameStatus::CHECK) {
            pgn << "+";
        }

        pgn << " ";

        // Adding newline after every full turn for readability
        if (move.playerColour == Colour::BLACK || &metadata == &chronologicalData.back()) {
            pgn << "\n";
        }
    }

    // Add the result at the end
    pgn << matchResult;

    return pgn.str();
}

inline bool columnExists(sqlite3* db, const std::string& tableName, const std::string& columnName) {
    bool exists = false;

    std::string query = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string currentColumnName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (currentColumnName == columnName) {
                exists = true;
                break;
            }
        }
    }

    sqlite3_finalize(stmt);
    return exists;
}

inline void addColumnIfNotExists(sqlite3* db, const std::string& tableName, const std::string& columnName, const std::string& columnType) {
    if (!columnExists(db, tableName, columnName)) {
        std::string alterCmd = "ALTER TABLE " + tableName + " ADD COLUMN " + columnName + " " + columnType + ";";

        char* errMsg;
        if (sqlite3_exec(db, alterCmd.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL Error: " << std::string(errMsg) << std::endl;
            sqlite3_free(errMsg);
        }
    }
}


#endif
