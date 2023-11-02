/**
 * @file GameState.h
 * @brief Declaration of the GameState class.
 * 
 * The GameState class manages the current game's state, including the chessboard, players, and game history. It provides a
 * robust interface for interacting with the game state, which includes state retrieval, time management, and game initialization.
 *
 * @author Bradley McGlynn
 * @date September 24th, 2023
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <unordered_map>
#include "ChessBoard.h"
#include "Player.h"
#include "Utilities.h"

class GameState {
public:
    /**
     * @brief Initializes the game state with the provided current and opponent players.
     */
    GameState(std::unique_ptr<Player> currentPlayer, std::unique_ptr<Player> opponentPlayer);

    /**
     * @brief Destructor that ensures proper memory cleanup.
     */
    ~GameState();

    /**
     * @brief Resets the game state to its initial configuration.
     */
    void resetGameState();

    /**
     * @brief Returns a mutable reference to the chessboard.
     */
    ChessBoard& getMutableBoard();
    
    /**
     * @brief Returns a pointer to the current player.
     */
    Player* getCurrentPlayer() const;

    /**
     * @brief Sets the current player.
     */
    void setCurrentPlayer(std::unique_ptr<Player>);

    /**
     * @brief Returns a pointer to the opponent player.
     */
    Player* getOpponentPlayer() const;

    /**
     * @brief Sets the opponent player.
     */
    void setOpponentPlayer(std::unique_ptr<Player>);

    /**
     * @brief Returns board state metadata and its corresponding indices.
     */
    std::pair<std::vector<BoardMetadata>, std::unordered_map<std::string, std::vector<int>>> getBoardStatesMetadata() const;
    
    /**
     * @brief Updates board state metadata.
     */
    void updateBoardStatesMetadata(std::string PGNstring, std::string timestamp);
    
    /**
     * @brief Checks if the allocated time for the game has run out.
     * @return True if time is out, otherwise false.
     */
    bool isTimeOut() const;
    
    /**
     * @brief Decreases the player's time by a given duration.
     */
    void decrementPlayerTime(const std::chrono::duration<double>& duration);
    
    /**
     * @brief Retrieves the remaining time for the game.
     * @return Duration representing remaining time.
     */
    std::chrono::duration<double> getRemainingTime() const;
    
    /**
     * @brief Provides access to the game's move history.
     * @return Vector of Moves representing the game history.
     */
    const std::vector<Move>& getGameHistory() const;

    /**
     * @brief Adds a move to the game history.
     */
    void addToGameHistory(const Move&);
    
    /**
     * @brief Provides access to the winner of the game, if any.
     * @return Pointer to the winning player.
     */
    Player* getWinner() const;
    
    /**
     * @brief Retrieves the start time point of the current turn.
     * @return Time point representing the start of the turn.
     */
    std::chrono::steady_clock::time_point getTurnStartTime() const;
    
    /**
     * @brief Updates the start time point of the current turn.
     */
    void setTurnStartTime(std::chrono::steady_clock::time_point);

    /**
     * @brief Provides access to the last move made in the game.
     * @return The last move made.
     */
    const Move& getLastMove() const;

    /**
     * @brief Updates the last move made in the game.
     */
    void setLastMove(const Move&);

    /**
     * @brief Retrieves the current turn number.
     * @return Current turn number.
     */
    int getTurnNumber() const;

    /**
     * @brief Increases the turn number by one.
     */
    void incrementTurnNumber();

    /**
     * @brief Sets the turn number.
     */
    void setTurnNumber(int);

    /**
     * @brief Provides access to the counter tracking turns without a capture or pawn move.
     * @return Current value of the counter.
     */
    int getNoCaptureOrPawnMoveCounter() const;

    /**
     * @brief Sets the counter that tracks turns without a capture or pawn move.
     */
    void setNoCaptureOrPawnMoveCounter(int);

    /**
     * @brief Retrieves the current game status.
     * @return The game's status.
     */
    GameStatus getGameStatus() const;

    /**
     * @brief Updates the game's status.
     */
    void setGameStatus(GameStatus);

    /**
     * @brief Switches the roles of the current and opponent players.
     */
    void switchTurns();

    /**
     * @brief Provides a string representation of the game's move history.
     * @return Game history in string format.
     */
    std::string getGameHistoryString() const;

    /**
     * @brief Updates the game's move history string representation.
     */
    void setGameHistoryString(const std::string& history);

    /**
     * @brief Converts a string representation of a move into a Move object.
     * @return A Move object representing the given string.
     */
    Move parseMoveString(const std::string& moveString) const;
    
private:
    std::unique_ptr<ChessBoard> board;                                   /** The current game board that manages all the pieces and their movements. */
    std::vector<BoardMetadata> chronologicalData;                        /** Maintains chronological metadata about the state of the board for various purposes, such as checking for threefold repetition. */
    std::unordered_map<std::string, std::vector<int>> boardStateIndices; /** Mapping that records on which turn specific board states occur, aiding in identifying repetitions. */
    Move lastMove;                                                       /** Keeps track of the last move that was made in the game. */
    int currentTurnNumber;                                               /** Counter for the current turn number, which increases after each move. */
    int noCaptureOrPawnMoveCounter;                                      /** Counter for moves that don't result in a pawn move or a piece capture. Helps in tracking 50-move rule for draws. */
    std::unique_ptr<Player> currentPlayer;                               /** Represents the player whose turn it is to move. */
    std::unique_ptr<Player> opponentPlayer;                              /** Represents the player waiting for their turn. */
    std::vector<Move> gameHistory;                                       /** List of moves that have been played in the current game. */
    std::string gameHistoryString;                                       /** A string-based representation of the game history, potentially useful for exporting, sharing, or visualizing the game's moves. */
    std::chrono::steady_clock::time_point turnStartTime;                 /** Timestamp indicating when the current player's turn started. Useful for games with time constraints. */
    GameStatus gameStatus;                                               /** Represents the current status of the game (e.g., in progress, checkmate, stalemate). */
};

#endif // GAME_STATE_H
