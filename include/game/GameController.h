#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

/**
 * Author: Bradley McGlynn
 * Description: Declaration of the GameController class.
 * This class acts as a mediator between the game's logic and the game's state,
 * managed by the GameState class, providing a unified
 * interface for the UI to interact with.
 * It is responsible for controlling the flow of the game,
 * managing turns, and handling user interactions such
 * as moves, undos, redos, saves, loads, and resignations.
 * Date: September 28th, 2023
 */

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <math.h>
#include <utility>
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <cstdlib>
#include <regex>
#include "GameState.h"
#include "Piece.h"
#include "Piece.h"
#include "DataUtil.h"
#include "SQLUtil.h"
#include "PGNUtil.h"
#include "ConversionUtil.h"

class GameController {
public:
    /** Constructs the GameController with specified players and database name. */
    GameController(std::unique_ptr<Player> whitePlayer, std::unique_ptr<Player> blackPlayer, std::string dbname, bool testMode = false);

    /** Destructor for the GameController class. */
    virtual ~GameController();

    /** Initializes a new game with given players. */
    void startGame(std::unique_ptr<Player> whitePlayer, std::unique_ptr<Player> blackPlayer);

    /** Attempts to make a move between two positions and updates the game state. */
    bool makeMove(const Position& start, const Position& end);

    /** Retrieves all possible moves for a piece at a given position. */
    std::vector<Position> getPossibleMoves(const Position& position) const;

    /** Undoes the last move.*/
    bool undoMove();

    /** Redoes the last undone move. */
    bool redoMove();

   /** Saves the current game state. */
    bool saveGame();

    /** Returns the number of saved games for two given users */
    int getNumSaves(User* user1, User* user2);

    /** Loads a game state from the database using the game ID. */
    bool loadGame(int gameID, User* user1, User* user2);

    /** Removes a game from the database using the game ID. */
    bool removeGame(int gameID);

    /**
     * @brief Retrieves the game history.
     * Used by the UI to display the move history during a game
     * @return Vector of Move objects representing the game's move history.
     */
    const std::vector<Move>& getGameHistory() const;

    /** Handles a player's resignation. */
    void playerResigns(Player* player);

    /** Gets the piece at a specified position */
    Piece* getPieceAtPosition(const Position& position) const;

    /** Validates if a move between two positions is possible. */
    bool validateMove(const Position& start, const Position& end) const;

    /** Attempts to make a move without committing it to see if it would remove the King from check. */
    bool tryMove(const Position& start, const Position& end) const;

    /** Reduces the current player's remaining time by the specified duration. */
    void decrementPlayerTime(const std::chrono::duration<double>& duration);

    /** Retrieves the current player. */
    Player* getCurrentPlayer() const;

    /** Retrieves the opponent player. */
    Player* getOpponentPlayer() const;

    /** Retrieves the winner of the game. */
    Player* getWinner() const;

    /** Retrieves the current status of the game. */
    GameStatus getGameStatus() const;

    /** Retrieves a mutable reference to the chessboard. */
    ChessBoard& getMutableBoard();

    /** Retrieves the current game state. */
    GameState* getGameState() const;

    /** Retrieves the remaining time for the current player's turn. */
    std::chrono::duration<double> getRemainingTime() const;

    /** Retrieves the board state metadata and corresponding indices. */
    std::pair<std::vector<BoardMetadata>, std::unordered_map<std::string, std::vector<int>>> getBoardStatesMetadata() const;

    /** Determines if the current game state is a stalemate. */
    bool isStalemate() const;

    /** Checks if the current player's king is in check. */
    bool isKingInCheck() const;

    /** Retrieves the pieces involved in current check state */
    std::vector<Position> getCheckPieces() const;

    /** Determines if the current player's king is in checkmate. */
    bool isKingInCheckmate() const;

    /** Marks the current game as ended. */
    void endGame();

    /** Updates the last move made in the game. */
    void setLastMove(const Move& move);

    /** Sets the counter for turns since the last capture or pawn move. */
    void setNoCaptureOrPawnMoveCounter(int count);

    /** Updates the status of the game. */
    void setGameStatus(GameStatus status);


    /** Updates the board states metadata. */
    void updateBoardStatesMetadata(std::string PGNstring, std::string timestamp);

    /** Updates the game's status based on the current state. */
    void updateGameStatus();

    /** Alternates the roles of current and opponent pla */
    void switchTurns();

    /** Resets the game state to its initial state. */
    void resetGameState();

    /** Promotes a pawn to another piece type. */
    bool promotePawn(const Position& position, PieceType type);

    /** Retrieves the current turn number. */
    int getTurnNumber() const;

    /** Retrieves the last move made in the game. */
    const Move& getLastMove() const;

    /* Retrieves the game's move history as a string. */
    std::string getGameHistoryString() const;

    /** Gets the state of test mode. */
    bool getTestMode();

    /** Gets all the loadable games for 2 users */
    std::vector<std::unordered_map<std::string, std::string>> getLoadableGames(User* user1, User* user2);

    /** Calculates the change in a player's Elo based on if they won or lost. Returns both values. */
    std::pair<double, double> getEloChange(double currentElo, double opponentElo);

private:
    static constexpr double eloConstant = 20.0;        /** Constant used in elo calculation. */ 

    std::unique_ptr<GameState> gameState;          /** Holds the current state of the chess game including board, pieces, moves, etc. */
    int gameID;                                     /** A unique identifier for this game instance. Useful for databases, logging, or multi-game management. */
    sqlite3* db;                                    /** A pointer to the SQLite database connection for persisting game data. */
    bool testMode;                                  /** State denoting test mode. */

    /** Generates a new game ID. */
    int generateGameID();

    /** Checks if the time limit for the current turn/player is exceeded. */
    bool isTimeOut() const;

    /** Increments the turn number by one. */
    void incrementTurnNumber();

    /** Sets the turn number. */
    void setTurnNumber(int turn);

    /** Appends a move to the game history. */
    void addToGameHistory(const Move& move);

    /** Retrieves the number of turns since the last capture or pawn move. */
    int getNoCaptureOrPawnMoveCounter() const;

    /** Sets the opponent player. */
    void setOpponentPlayer(std::unique_ptr<Player> player);

    /** Sets the current player. */
    void setCurrentPlayer(std::unique_ptr<Player> player);

    /** Determines if the current player is in a resigning state. */
    bool isPlayerResigning() const;

    /** Sets the current player's state to resigning. */
    void setPlayerResigning();

    /** Retrieves the start time point of the current turn. */
    std::chrono::steady_clock::time_point getTurnStartTime() const;

    /** Sets the start time for the current turn. */
    void setTurnStartTime(std::chrono::steady_clock::time_point timePoint);

    /** Sets the game's move history using a string. */
    void setGameHistoryString(const std::string& history);

    /** Calculates the probability of the current player winning. */
    double chanceOfWinning(double currentElo, double opponentElo);
    
    /** Calculates the change in a player's Elo based on if they won or lost. Returns one value. */
    double getEloChange(double currentElo, double opponentElo, double score);
};

#endif // GAME_CONTROLLER_H
