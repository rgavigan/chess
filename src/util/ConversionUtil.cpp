/**
 * @file ConversionUtil.cpp
 * @brief This is the implementation file for the ConversionUtil class. 
 * This class defines conversion methods between various data structures used in the application.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#include "ConversionUtil.h"

/** 
 * @brief Get the opposite colour of the given colour.
 * @param colour Colour.
 * @return Opposite colour.
*/
Colour ConversionUtil::oppositeColour(Colour colour) {
    return (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
}

/**
 * @brief Turns the player's last move into a readable string.
 * @param move Last move made.
 * @return Move as a string.
 */
std::string ConversionUtil::turnMoveIntoString(const Move& move) {
    std::string moveString = "";
    std::unordered_map<int, char> columnToChar = {{0, 'a'}, {1, 'b'}, {2, 'c'}, {3, 'd'}, {4, 'e'}, {5, 'f'}, {6, 'g'}, {7, 'h'}};

    std::string startRow = std::to_string(abs(move.start.row - 8));
    std::string endRow =  std::to_string(abs(move.end.row - 8));

    moveString += columnToChar[move.start.col] + startRow + columnToChar[move.end.col] + endRow;

    return moveString;
}

/**
 * @brief Turns a string into a move.
 * 
 * @param move the mvoe as a string
 * @return the nive
 */
Move ConversionUtil::turnStringIntoMove(const std::string& move) {
    Move newMove;
    std::unordered_map<char, int> charToColumn = {{'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}};

    newMove.start.col = charToColumn[move[0]];
    newMove.start.row = 8 - (move[1] - '0');
    newMove.end.col = charToColumn[move[2]];
    newMove.end.row = 8 - (move[3] - '0');

    return newMove;
}

/**
 * @brief Converts a position into a string in Chess notation.
 * @param pos Position.
 * @return Position in chess notation, as a string.
*/
std::string ConversionUtil::positionToChessNotation(const Position& pos) {
    char file = 'a' + pos.col;
    int rank = 8 - pos.row;
    return std::string(1, file) + std::to_string(rank);
}

/**
 * @brief Converts a Piece to a chess notation string.
 * @param piece Piece.
 * @return Piece as a chess notation string.
*/
std::string ConversionUtil::pieceTypeToNotation(PieceType piece) {
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

/**
 * @brief Converts a string representation of a game status to a GameStatus object.
 * @param statusStr Game status as a string.
 * @return Game status.
*/
GameStatus ConversionUtil::stringToGameStatus(const std::string& statusStr) {
    const std::map<std::string, GameStatus> stringToStatusMap = {
        {"ONGOING", GameStatus::ONGOING},
        {"CHECK", GameStatus::CHECK},
        {"PROMPTDRAW", GameStatus::PROMPTDRAW},
        {"CHECKMATE", GameStatus::CHECKMATE},
        {"STALEMATE", GameStatus::STALEMATE},
        {"DRAW", GameStatus::DRAW},
        {"RESIGN", GameStatus::RESIGN}
    };

    auto it = stringToStatusMap.find(statusStr);
    if (it != stringToStatusMap.end()) {
        return it->second;
    }

    std::cerr << "Unknown game status string: " << statusStr << std::endl;
    return GameStatus::ONGOING;
}

/**
 * @brief Converts a game status object into a string representation.
 * @param status Game status.
 * @return Game status as a string.
*/
std::string ConversionUtil::gameStatusToString(GameStatus status) {
    const std::map<GameStatus, std::string> statusToStringMap = {
        {GameStatus::ONGOING, "ONGOING"},
        {GameStatus::CHECK, "CHECK"},
        {GameStatus::PROMPTDRAW, "PROMPTDRAW"},
        {GameStatus::CHECKMATE, "CHECKMATE"},
        {GameStatus::STALEMATE, "STALEMATE"},
        {GameStatus::DRAW, "DRAW"},
        {GameStatus::RESIGN, "RESIGN"}
    };

    auto it = statusToStringMap.find(status);
    if (it != statusToStringMap.end()) {
        return it->second;
    }

    std::cerr << "Unknown game status: " << static_cast<int>(status) << std::endl;
    return "UNKNOWN";
}
