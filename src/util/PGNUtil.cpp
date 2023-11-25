/**
 * @file PGNUtil.cpp
 * @brief This is the implementation file for the PGNUtil class. This class defines methods for use in PGN file generation.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#include "PGNUtil.h"

/**
 * @brief Generates a PGN file from a given game state.
 * @param username1 Exporter username.
 * @param whitePlayer White player.
 * @param blackPlayer Black player.
 * @param gameStatus Current game status.
 * @param chronologicalData Vector of board metadata states.
 * @return PGN file contents, as a string.
*/
std::string PGNUtil::generatePGN(
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
            pgn << ConversionUtil::pieceTypeToNotation(move.pieceMoved);

            // For pawns capturing, we include the starting file
            if (move.pieceMoved == PieceType::PAWN && move.pieceCaptured != PieceType::NONE) {
                pgn << static_cast<char>('a' + move.start.col);
            }

            pgn << (move.pieceCaptured != PieceType::NONE ? "x" : "")
                << ConversionUtil::positionToChessNotation(move.end);

            // Pawn promotion
            if (move.promotionPiece != PieceType::NONE) {
                pgn << "=" << ConversionUtil::pieceTypeToNotation(move.promotionPiece);
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


/**
 * @brief Gets the current date as a string.
 * @return Current date string.
*/
std::string PGNUtil::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    std::stringstream ss;
    ss << std::put_time(parts, "%Y-%m-%d");
    return ss.str();
}