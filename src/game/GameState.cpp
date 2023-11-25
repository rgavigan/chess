/**
 * @file GameState.cpp
 * @brief Implementation of the GameState class.
 *
 * This file contains the implementation of the methods declared in GameState.h.
 * The GameState class is responsible for managing and maintaining the current state of the chess game,
 * including the board state, the players, the game history, and the game status.
 *
 * @author Bradley McGlynn
 * @date September 28th, 2023
*/

#include <algorithm>
#include "GameState.h"
#include "Piece.h"

/**
 * @brief Constructs a GameState object and initializes the game state.
 * @param currentPlayer The current player.
 * @param opponentPlayer The opponent player.
 */
GameState::GameState(std::unique_ptr<Player> currentPlayer, std::unique_ptr<Player> opponentPlayer)
    : board(std::make_unique<ChessBoard>()),
      lastMove(Position(0,0), Position(0,0), PieceType::NONE, Colour::WHITE),
      currentTurnNumber(1),
      noCaptureOrPawnMoveCounter(0),
      currentPlayer(std::move(currentPlayer)),
      opponentPlayer(std::move(opponentPlayer)),
      turnStartTime(std::chrono::steady_clock::now()),
      gameStatus(GameStatus::ONGOING){
    }

/**
 * @brief Destructor for the GameState class.
 */
GameState::~GameState() {}

/**
 * @brief Resets the game state to its initial state.
 * @param testMode If test mode is on, default false.
 */
void GameState::resetGameState(bool testMode) {
    // Resetting the ChessBoard
    board->initializeBoard(testMode);

    // Clearing the board state metadata
    chronologicalData.clear();

    // Clearing the board state indices
    boardStateIndices.clear();

    // Resetting the last move
    lastMove = Move(Position(0,0), Position(0,0), PieceType::NONE, Colour::WHITE);

    // Resetting the current turn number
    currentTurnNumber = 1;

    // Resetting the no capture or pawn move counter
    noCaptureOrPawnMoveCounter = 0;

    // Resetting the players
    currentPlayer = nullptr;
    opponentPlayer = nullptr;

    // Clearing the game history
    gameHistory.clear();

    // Resetting the turn start time
    turnStartTime = std::chrono::steady_clock::now();

    // Resetting the game status
    gameStatus = GameStatus::ONGOING;

    // reset the gameHistoryString
    gameHistoryString = "";


}

/**
 * @brief Returns a pointer to board for use inside of GameLogic.
 * @return pointer to ChessBoard board.
 */
ChessBoard& GameState::getMutableBoard() {
    return *board;
}

/**
 * @brief Decrements the current player's remaining time based on the time they took for their turn.
 * @param duration Represents the time spent on the turn.
 */
void GameState::decrementPlayerTime(const std::chrono::duration<double>& duration) {
    currentPlayer->decrementTime(duration);
    if (currentPlayer->getTimeLeft() <= std::chrono::seconds(0)) {
        setGameStatus(GameStatus::TIMEOUT);
    }
}

/**
 * @brief Gets the remaining time for the current player's turn.
 * @return std::chrono::duration<double> representing the remaining time.
 */
std::chrono::duration<double> GameState::getRemainingTime() const {
    return currentPlayer->getTimeLeft();
}

/**
 * @brief Checks if the current player's time has run out.
 * @return true if the time has run out, false otherwise.
 */
bool GameState::isTimeOut() const {
   return currentPlayer->getTimeLeft() <= std::chrono::seconds(0);
}

/**
 * @brief Gets the current player.
 * @return Pointer to the current Player object.
 */
Player* GameState::getCurrentPlayer() const {
    return currentPlayer.get();
}

/**
 * @brief Sets the current player.
 * @param currentPlayer Unique pointer to the player object to set as the current player.
 */
void GameState::setCurrentPlayer(std::unique_ptr<Player> currentPlayer) {
    this->currentPlayer = std::move(currentPlayer);
}

/**
 * @brief Gets the opponent player.
 * @return Pointer to the opponent Player object.
 */
Player* GameState::getOpponentPlayer() const {
    return opponentPlayer.get();
}

/**
 * @brief Sets the opponent player.
 * @param opponentPlayer Unique pointer to the player object to set as the opponent player.
 */
void GameState::setOpponentPlayer(std::unique_ptr<Player> opponentPlayer) {
    this->opponentPlayer = std::move(opponentPlayer);
}

/**
 * @brief Gets the board states metadata and the indices of the board states.
 * @return Pair containing vectors of the board states metadata and the indices of the board states.
 */
std::pair<std::vector<BoardMetadata>, std::unordered_map<std::string, std::vector<int>>> GameState::getBoardStatesMetadata() const {
    return {chronologicalData, boardStateIndices};
}

/**
 * @brief Updates the board states metadata, adding it in chronological order as well as keeping track of that board states occurrences.
 */
void GameState::updateBoardStatesMetadata(std::string PGNstring, std::string timestamp) {
    std::string currentState = board->getBoardStateAsString();

    BoardMetadata metadata(currentPlayer.get(), opponentPlayer.get(),
        currentPlayer->getColour(), opponentPlayer->getColour(),
        currentState, currentTurnNumber, gameStatus, lastMove,
        PGNstring, timestamp);

    // Add to the main vector
    chronologicalData.emplace_back(metadata);

    // Add the index to the map
    boardStateIndices[metadata.boardState].emplace_back(chronologicalData.size() - 1);
}

/**
 * @brief Gets the vector of game history.
 * @return Vector of Move objects containing game history.
 */
const std::vector<Move>& GameState::getGameHistory() const {
    return gameHistory;
}

/**
 * @brief Adds a move to the game history.
 * @param move The Move object representing the move to add.
 */
void GameState::addToGameHistory(const Move& move) {
    gameHistory.emplace_back(move);
    gameHistoryString += ConversionUtil::turnMoveIntoString(move) + " ";
    std::cout << "Add to game history string: " << gameHistoryString << std::endl;
}

/**
 * @brief Gets the winner of the match.
 * @return Pointer to the Player object representing the winner, or nullptr if there's no winner yet.
 */
Player* GameState::getWinner() const {
    if (gameStatus == GameStatus::CHECKMATE) {
        return currentPlayer->getIsInCheck() ? opponentPlayer.get() : currentPlayer.get();
    }

    else if (gameStatus == GameStatus::RESIGN) {
        return currentPlayer->getIsResigning() ? opponentPlayer.get() : currentPlayer.get();
    }

    else if (gameStatus == GameStatus::TIMEOUT) {
        return currentPlayer->getTimeLeft() <= std::chrono::seconds(0) ? opponentPlayer.get() : currentPlayer.get();
    }

    return nullptr;
}

/**
 * @brief Gets the time that the player's turn started on.
 * @return std::chrono::steady_clock::time_point representing the turn start time.
 */
std::chrono::steady_clock::time_point GameState::getTurnStartTime() const {
    return turnStartTime;
}

/**
 * @brief Sets the time that the player's turn started on.
 * @param turnStartTime std::chrono::steady_clock::time_point representing the turn start time.
 */
void GameState::setTurnStartTime(std::chrono::steady_clock::time_point turnStartTime) {
    this->turnStartTime = turnStartTime;
}

/**
 * @brief Gets the previous move made in the game.
 * @return Move object representing the last move made.
 */
const Move& GameState::getLastMove() const {
    return lastMove;
}

/**
 * @brief Sets the previous move made in the game.
 * @param lastMove Move object representing the last move made.
 */
void GameState::setLastMove(const Move& lastMove) {
    this->lastMove = lastMove;
}

/**
 * @brief Gets the current turn number.
 * @return Integer representing the current turn number.
 */
int GameState::getTurnNumber() const {
    return currentTurnNumber;
}

/**
 * @brief Increments the current turn number.
 */
void GameState::incrementTurnNumber() {
    currentTurnNumber++;
}

/**
 * @brief Sets the current turn number.
 * @param turns Integer representing the number of turns.
 */
void GameState::setTurnNumber(int turns) {
    currentTurnNumber = turns;
}

/**
 * @brief Gets the no capture or pawn move counter.
 * @return Integer representing the no capture or pawn move counter.
 */
int GameState::getNoCaptureOrPawnMoveCounter() const {
    return noCaptureOrPawnMoveCounter;
}

/**
 * @brief Sets the no capture or pawn move counter.
 * @param noCaptureOrPawnMoveCounter Integer representing the no capture or pawn move counter.
 */
void GameState::setNoCaptureOrPawnMoveCounter(int noCaptureOrPawnMoveCounter) {
    this->noCaptureOrPawnMoveCounter = noCaptureOrPawnMoveCounter;
}

/**
 * @brief Gets the game status.
 * @return GameStatus enum representing the game status.
 */
GameStatus GameState::getGameStatus() const {
    return gameStatus;
}

/**
 * @brief Sets the game status and adjusts player check and resigning states accordingly.
 * @param gameStatus GameStatus enum representing the desired game status.
 */
void GameState::setGameStatus(GameStatus gameStatus) {
    this->gameStatus = gameStatus;
    if (gameStatus == GameStatus::CHECKMATE) {
        // If the game is over, set the winner
        currentPlayer->setIsInCheck(true);
        opponentPlayer->setIsInCheck(false);
    }

    else if (gameStatus == GameStatus::STALEMATE) {
        // Neither player in check when the game is a draw
        currentPlayer->setIsInCheck(false);
        opponentPlayer->setIsInCheck(false);
    }

    else if (gameStatus == GameStatus::ONGOING) {
        // If the game is ongoing, neither player is in check
        currentPlayer->setIsInCheck(false);
        opponentPlayer->setIsInCheck(false);
    }

    else if (gameStatus == GameStatus::RESIGN) {
        // If current player resigns, set opponent as winner via check
        currentPlayer->setIsResigning();
        currentPlayer->setIsInCheck(true);
        opponentPlayer->setIsInCheck(false);
    }

    else if (gameStatus == GameStatus::DRAW) {
        // If the game is a draw, neither player is in check
        currentPlayer->setIsInCheck(false);
        opponentPlayer->setIsInCheck(false);
    }

    else if (gameStatus == GameStatus::CHECK) {
        // Make sure correct player is in check
        currentPlayer->setIsInCheck(true);
        opponentPlayer->setIsInCheck(false);
    }
}

/**
 * @brief Switches the roles of the current and opponent players.
 */
void GameState::switchTurns() {
    if (currentPlayer->getColour() == Colour::BLACK) {
        incrementTurnNumber(); // Increment the turn number once both players end their turns, i.e after the black player ends their turn
    }
    std::swap(currentPlayer, opponentPlayer);
}

/**
 * @brief Gets the game history string.
 * @return String representing the game history.
 */
std::string GameState::getGameHistoryString() const {
    return gameHistoryString;
}

/**
 * @brief Sets the game history string.
 * @param gameHistoryString String representing the desired game history.
 */
void GameState::setGameHistoryString(const std::string& gameHistoryString) {
    this->gameHistoryString = gameHistoryString;

    // Split the game history string by spaces
    std::stringstream ss(gameHistoryString);
    std::string moveString;
    std::vector<Move> moves;
    while (ss >> moveString) {
        moves.emplace_back(ConversionUtil::turnStringIntoMove(moveString));
    }

    // Go through the moves and add to game history
    for (const auto& move : moves) {
       gameHistory.emplace_back(move);
    }
}   
