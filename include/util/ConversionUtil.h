/**
 * @file ConversionUtil.h
 * @brief This is the header file for the ConversionUtil class. 
 * This class defines conversion methods between various data structures used in the application.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#ifndef CONVERSIONUTIL_H
#define CONVERSIONUTIL_H

#include "DataUtil.h"

class ConversionUtil {
public:
    static Colour oppositeColour(Colour colour);
    static std::string turnMoveIntoString(const Move& move);
    static Move turnStringIntoMove(const std::string& move);
    static std::string positionToChessNotation(const Position& pos);
    static std::string pieceTypeToNotation(PieceType piece);
    static GameStatus stringToGameStatus(const std::string& statusStr);
    static std::string gameStatusToString(GameStatus status);
};

#endif // CONVERSIONUTIL_H