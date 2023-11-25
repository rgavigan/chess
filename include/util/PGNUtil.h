/**
 * @file PGNUtil.h
 * @brief This is the header file for the PGNUtil class. This class defines methods for use in PGN file generation.
 * @author Bradley McGlynn
 * @author Chris Hosang
 * @date November 22nd, 2023
*/

#ifndef PGNUTIL_H
#define PGNUTIL_H

#include "DataUtil.h"
#include "ConversionUtil.h"

class PGNUtil {
public:
    static std::string generatePGN(const std::string& username1, const std::string& whitePlayer, const std::string& blackPlayer, const GameStatus gameStatus, const std::vector<BoardMetadata>& chronologicalData);
    static std::string getCurrentDate();
};

#endif // PGNUTIL_H