/**
 * @file GameController.cpp
 * @brief Implementation of the GameController class.
 * 
 * This file contains the implementation of the methods declared in GameController.h.
 * The GameController class serves as the mediator between the game's logic and the game's state, managed by the GameState class.
 * It provides a unified interface for the UI to interact with, controlling the flow of the game, managing turns, and handling user 
 * interactions such as moves, undos, redos, saves, loads, and resignations. This ensures a coherent interaction between the game's 
 * logic and state. The class also acts as the core engine for the chess game, handling game rules, validating moves, and executing 
 * game actions while interacting with the GameState to access and modify the game's current state.
 *
 * @author Bradley McGlynn, Chris Hosang
 * @date September 28th, 2023
 */

#include <algorithm>
#include <chrono>
#include <cerrno>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
#include "PieceFactory.h"
#include "Player.h"
#include "GameController.h"
#include "UserManager.h"

/**
 * @brief Constructs a GameController object.
 */
GameController::GameController(std::unique_ptr<Player> whitePlayer, std::unique_ptr<Player> blackPlayer, std::string dbname) :
    gameState(std::make_unique<GameState>(std::move(whitePlayer), std::move(blackPlayer))), stockfishDifficulty(Difficulty::HARD) {
    // Open SQl database 
    if(sqlite3_open(dbname.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << std::string(sqlite3_errmsg(db)) << std::endl;
        sqlite3_close(db);     
    }

    // Initialize tables 
    std::string sql =   "CREATE TABLE IF NOT EXISTS GAMESTATE("
                        "GAMEID INTEGER,"                       // Unique ID assigned to each game 
                        "BOARDSTATE TEXT NOT NULL,"             // String representation of chess board 
                        "TURNNUMBER INTEGER,"                   // Turn number 
                        "GAMESTATUS TEXT,"                      // Game status (ongoing, check etc.) 
                        "P1MINS REAL,"                          // Match timers, as doubles 
                        "P2MINS REAL);"

                        "CREATE TABLE IF NOT EXISTS GAMEDATA("
                        "GAMEID INTEGER,"                       
                        "USERNAME1 TEXT NOT NULL,"              // Usernames, as strings 
                        "USERNAME2 TEXT NOT NULL,"
                        "COLOUR1 INTEGER,"                      // Player colours, as 0: white, 1: black 
                        "COLOUR2 INTEGER);";          

    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr <<  "SQL Error: " << std::string(sqlite3_errmsg(db)) << std::endl;
        sqlite3_close(db);
    }

    addColumnIfNotExists(db, "GAMEDATA", "PGN", "TEXT");
    addColumnIfNotExists(db, "GAMEDATA", "TIMESTAMP", "TEXT");

    // Initialize Game ID 
    gameID = generateGameID();
}
 
/**
 * @brief Destructor for the GameController class. 
 * Cleans up any resources used by the object.
 */
GameController::~GameController() {
    sqlite3_close(db);        
}

/**
 * @brief Initializes the game state and game logic.
 * @param whitePlayer A pointer to the white player.
 * @param blackPlayer A pointer to the black player.
 */
void GameController::startGame(std::unique_ptr<Player> whitePlayer, std::unique_ptr<Player> blackPlayer) {
    // Start the game by initializing game components 
    // and setting up the initial state and logic 
    gameState->resetGameState();

    gameState->setCurrentPlayer(std::move(whitePlayer));
    gameState->setOpponentPlayer(std::move(blackPlayer));
    gameID = generateGameID();
}

/**
 * @brief Undoes the last move made in the game.
 * @return true if the move was successfully undone, false otherwise.
 */
bool GameController::undoMove() {
    // Logic to undo the last move 
    return true;
}

/**
 * @brief Redoes the last move undone in the game.
 * @return true if the move was successfully redone, false otherwise.
 */
bool GameController::redoMove() {
    // Logic to redo the last undone move 
    return false;
}

/**
 * @brief Saves the current game state to a file.
 * @return true if the game was successfully saved, false otherwise.
 */
bool GameController::saveGame() {
    // Check if game data has already been saved 
    std::string duplicateq = "SELECT COUNT(*) FROM GAMEDATA WHERE GAMEID = ?;";
    sqlite3_stmt* duplicate_stmt;

    int count = -1;
    if(sqlite3_prepare_v2(db, duplicateq.c_str(), -1, &duplicate_stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(duplicate_stmt, 1, gameID);
        if(sqlite3_step(duplicate_stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(duplicate_stmt, 0);
        }
        sqlite3_finalize(duplicate_stmt);
    }

    // If game data doesn't exist yet, create it 
    if(count <= 0) {
        std::string username1 = getCurrentPlayer()->getUser().getUsername();
        std::string username2 = getOpponentPlayer()->getUser().getUsername();

        int col1 = getCurrentPlayer()->getColour() == Colour::WHITE ? 0 : 1;
        int col2 = 1 - col1;

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&now_c); 

        std::string whitePlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? username1 : username2;

        std::string blackPlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? username2 : username1;

        std::string pgn = generatePGN(getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, getGameStatus(), getBoardStatesMetadata().first);

        std::string dataq = "INSERT INTO GAMEDATA(GAMEID, USERNAME1, USERNAME2, COLOUR1, COLOUR2, PGN, TIMESTAMP) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* data_stmt;

        if(sqlite3_prepare_v2(db, dataq.c_str(), -1, &data_stmt, 0) == SQLITE_OK) {
            sqlite3_bind_int(data_stmt, 1, gameID);
            sqlite3_bind_text(data_stmt, 2, username1.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(data_stmt, 3, username2.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(data_stmt, 4, col1);
            sqlite3_bind_int(data_stmt, 5, col2);
            sqlite3_bind_text(data_stmt, 6, pgn.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(data_stmt, 7, timestamp.c_str(), -1, SQLITE_STATIC);

            if(sqlite3_step(data_stmt) == SQLITE_DONE) {
                sqlite3_finalize(data_stmt);
            }
        } 
    }

    // Save all board state metadata to database 
    std::string q = "INSERT INTO GAMESTATE(GAMEID, BOARDSTATE, TURNNUMBER, GAMESTATUS, P1MINS, P2MINS, P1MINS, P2MINS) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    auto stateData = getBoardStatesMetadata().first;
    for(int i = 0; i < stateData.size(); i++) {
        auto currState = stateData[i];
        GameStatus status = currState.gameStatus;
        std::string stringStatus = gameStatusToString(status);
        
        sqlite3_stmt* game_stmt;
        if(sqlite3_prepare_v2(db, q.c_str(), -1, &game_stmt, 0) == SQLITE_OK) {
            sqlite3_bind_int(game_stmt, 1, gameID);
            sqlite3_bind_text(game_stmt, 2, currState.boardState.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(game_stmt, 3, currState.turnNumber);
            sqlite3_bind_text(game_stmt, 4, stringStatus.c_str(), -1, SQLITE_STATIC);            
            sqlite3_bind_double(game_stmt, 5, gameState->getCurrentPlayer()->getTimeLeft().count());
            sqlite3_bind_double(game_stmt, 6, gameState->getOpponentPlayer()->getTimeLeft().count());            

            if(sqlite3_step(game_stmt) == SQLITE_DONE) {
                sqlite3_finalize(game_stmt);
            }
        }
    }

    return true;
}

/**
 * @brief Loads a game state from a file.
 * @param gameID ID of the game to be loaded.
 * @return true if the game was successfully loaded, false otherwise.
 */
bool GameController::loadGame(int gameID) {
    // Get saved game data 
    std::string dataq = "SELECT * FROM GAMEDATA WHERE GAMEID = ?;";
    sqlite3_stmt* data_stmt;

    if(sqlite3_prepare_v2(db, dataq.c_str(), -1, &data_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(data_stmt, 1, gameID);

    std::unique_ptr<Player> newP1, newP2;
    Colour newColour1, newColour2;

    int step = sqlite3_step(data_stmt); 
    if(step == SQLITE_ROW) {
        std::string newUsername1(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 1)));
        std::string newUsername2(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 2)));
        int col1 = sqlite3_column_int(data_stmt, 3);
        int col2 = sqlite3_column_int(data_stmt, 4);
        std::string pgn(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 5)));
        std::string timestamp(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 6)));
        
        User user1(newUsername1);
        User user2(newUsername2);

        if(col1 == 0) {
            newColour1 = Colour::WHITE;
            newColour2 = Colour::BLACK;
        } else {
            newColour1 = Colour::BLACK;
            newColour2 = Colour::WHITE;
        }
        
        newP1 = std::make_unique<Player>(user1, newColour1, std::chrono::minutes(0)); // Placeholder durations 
        newP2 = std::make_unique<Player>(user2, newColour2, std::chrono::minutes(0)); 

    } else {
        std::cerr << "Failed to fetch data from GAMEDATA: " << step << std::endl;
        sqlite3_finalize(data_stmt);
        return false;     
    }
    sqlite3_finalize(data_stmt);

    // Get saved game states  
    std::string stateq = "SELECT * FROM GAMESTATE WHERE GAMEID = ? ORDER BY TURNNUMBER;";
    sqlite3_stmt* game_stmt;

    if(sqlite3_prepare_v2(db, stateq.c_str(), -1, &game_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(game_stmt, 1, gameID);

    // Reset current game 
    gameState->resetGameState();
    gameState->setCurrentPlayer(std::move(newP1));
    gameState->setOpponentPlayer(std::move(newP2));

    // Replace current game with saved game states 
    while(sqlite3_step(game_stmt) == SQLITE_ROW) {
        gameState->getMutableBoard().initializeBoardFromString(reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 1)));
        setTurnNumber(sqlite3_column_int(game_stmt, 2));
        std::string status(reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 3))); 

        GameStatus gameStatus = stringToGameStatus(status);

        gameState->getCurrentPlayer()->setTimeLeft(std::chrono::duration<double>(sqlite3_column_double(game_stmt, 4)));
        gameState->getOpponentPlayer()->setTimeLeft(std::chrono::duration<double>(sqlite3_column_double(game_stmt, 5)));

        setGameStatus(gameStatus);


        // Update board state metadata 
        std::string whitePlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getCurrentPlayer()->getName() : getOpponentPlayer()->getName();

        std::string blackPlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getOpponentPlayer()->getName() : getCurrentPlayer()->getName();

        std::string pgn = generatePGN(getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, getGameStatus(), getBoardStatesMetadata().first);

        updateBoardStatesMetadata(pgn, getCurrentDate());
    }
    sqlite3_finalize(game_stmt);

    this->gameID = gameID;
    
    return true;
}

/**
 * @brief End the game and update the user statistics.
 */
void GameController::endGame() {
    //  Increment draws in case of draw 
    if (getGameStatus() == GameStatus::DRAW || getGameStatus() == GameStatus::STALEMATE) {
        getCurrentPlayer()->getUser().incrementDraws();
        getOpponentPlayer()->getUser().incrementDraws();
    }
    // Increment wins/losses in case of a win 
    else if (getGameStatus() == GameStatus::CHECKMATE || getGameStatus() == GameStatus::RESIGN) {
        // Update the underlying user with a win for the winner 
        Player* winner = getWinner();
        winner->getUser().incrementWins();

        // Update the underlying user with a loss for the loser 
        Player* loser = (winner == getCurrentPlayer()) ? getOpponentPlayer() : getCurrentPlayer();
        loser->getUser().incrementLosses();
    }
    // Create UserManager instance 
    UserManager userManager = UserManager("db/default.sql");
    
    // Update databases for user stats 
    if (getCurrentPlayer()->getUser().getUsername() != "") {
        userManager.updateUserStats(getCurrentPlayer()->getUser().getUsername(), getCurrentPlayer()->getUser().getWins(), getCurrentPlayer()->getUser().getLosses(), getCurrentPlayer()->getUser().getDraws());
    }
    if (getOpponentPlayer()->getUser().getUsername() != "") {
        userManager.updateUserStats(getOpponentPlayer()->getUser().getUsername(), getOpponentPlayer()->getUser().getWins(), getOpponentPlayer()->getUser().getLosses(), getOpponentPlayer()->getUser().getDraws());
    }
}

/**
 * @brief Generates a unique game ID and adds it to both player's game lists.
 * @return Generated game ID, or -1 if an SQL error occurred.
 */
int GameController::generateGameID() {
    // Find highest existing game ID  
    std::string q = "SELECT MAX(GAMEID) FROM GAMEDATA;";
    sqlite3_stmt* stmt;
    
    if(sqlite3_prepare_v2(db, q.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Create a new game ID based on the highest existing one 
    int id = sqlite3_step(stmt) == SQLITE_ROW ? sqlite3_column_int(stmt, 0) + 1 : 1;
    sqlite3_finalize(stmt);
            
    // Add new ID to each player's games list 
    gameState->getCurrentPlayer()->getUser().addGame(id);
    gameState->getOpponentPlayer()->getUser().addGame(id);
    
    return id;
}

/**
 * @brief Retrieves the current player of the game.
 * @return A pointer to the current player.
 */
Player *GameController::getCurrentPlayer() const {
    return gameState->getCurrentPlayer();
}

/**
 * @brief Returns the mutable board instance
 * 
 * @return a mutable board instance
 */
ChessBoard &GameController::getMutableBoard() {
    return gameState->getMutableBoard();
}

Piece *GameController::getPieceAtPosition(const Position &position) const {
    return gameState->getMutableBoard().getPieceAtPosition(position);
}

/**
 * @brief Sets the current player of the game.
 * @param player A pointer to the player to set as the current player.
 */
void GameController::setCurrentPlayer(std::unique_ptr<Player> player) {
    gameState->setCurrentPlayer(std::move(player));
}

/**
 * @brief Retrieves the opponent player of the game.
 * @return A pointer to the opponent player.
 */
Player* GameController::getOpponentPlayer() const {
    return gameState->getOpponentPlayer();
}

/**
 * @brief Sets the opponent player of the game.
 * @param player A pointer to the player to set as the opponent player.
 */
void GameController::setOpponentPlayer(std::unique_ptr<Player> player) {
    gameState->setOpponentPlayer(std::move(player));
}

/**
 * @brief Gets the board states metadata and the indices of the board states.
 * @return pair of vectors containing the board states metadata and the indices of the board states.
 */
std::pair<std::vector<BoardMetadata>, std::unordered_map<std::string, std::vector<int>>> GameController::getBoardStatesMetadata() const {
    return gameState->getBoardStatesMetadata();
}

/**
 * @brief Updates the board states metadata, adding it in chronological order as well as keeping track of that board states occurrences.
 */
void GameController::updateBoardStatesMetadata(std::string PGNstring, std::string timestamp) {
    gameState->updateBoardStatesMetadata(PGNstring, timestamp);
}

/**
 * @brief Checks if the current player has run out of time.
 * @return true if the time is out, false otherwise.
 */
bool GameController::isTimeOut() const {
    return gameState->isTimeOut();
}

/**
 * @brief Resets the game state
 * 
 */
void GameController::resetGameState() {
    gameState->resetGameState();
}

/**
 * @brief Decrements the time available to the current player.
 * @param duration The amount of time to decrement the player's time by as a std::chrono::duration<double>.
 */
void GameController::decrementPlayerTime(const std::chrono::duration<double>& duration) {
    gameState->decrementPlayerTime(duration);
}

/**
 * @brief Retrieves the remaining time available to the current player.
 * @return The remaining time as a std::chrono::duration<double>.
 */
std::chrono::duration<double> GameController::getRemainingTime() const {
    return gameState->getRemainingTime();
}

/**
 * @brief Retrieves the history of moves made during the game.
 * @return A constant reference to a vector of Move objects representing the game history.
 */
const std::vector<Move>& GameController::getGameHistory() const {
    return gameState->getGameHistory();
}

/**
 * @brief Handle a player resigning a match / running out of time
 * @param player the player resigning
 */
void GameController::playerResigns(Player *player) {
    player->setIsResigning();
    setGameStatus(GameStatus::RESIGN);
}

/**
 * @brief Adds a move to the game history.
 * @param move The move to add to the game history.
 */
void GameController::addToGameHistory(const Move& move) {
    gameState->addToGameHistory(move);
}

/**
 * @brief Determines the winner of the game.
 * @return A pointer to the Player object representing the winner, or nullptr if there is no winner yet.
 */
Player* GameController::getWinner() const {
    return gameState->getWinner();
}

/**
 * @brief Retrieves the start time of the current player's turn.
 * @return The start time of the current player's turn as a std::chrono::steady_clock::time_point.
 */
std::chrono::steady_clock::time_point GameController::getTurnStartTime() const {
    return gameState->getTurnStartTime();
}

/**
 * @brief Sets the start time of the current player's turn.
 * @param time The start time of the current player's turn as a std::chrono::steady_clock::time_point.
 */
void GameController::setTurnStartTime(std::chrono::steady_clock::time_point time) {
    gameState->setTurnStartTime(time);
}

/**
 * @brief Retrieves the last move made in the game.
 * @return A constant reference to the last Move object made in the game.
 */
const Move& GameController::getLastMove() const {
    return gameState->getLastMove();
}

/**
 * @brief Sets the last move made in the game.
 * @param move The last move made in the game as a Move object.
 */
void GameController::setLastMove(const Move& move) {
    gameState->setLastMove(move);
}

/**
 * @brief Retrieves the current turn number.
 * @return The current turn number as an integer.
 */
int GameController::getTurnNumber() const {
    return gameState->getTurnNumber();
}

/**
 * @brief Increments the current turn number.
 */
void GameController::incrementTurnNumber() {
    gameState->incrementTurnNumber();
}

/**
 * @brief Sets the current turn number.
 * @param turns Number representing the current turn number.
 */
void GameController::setTurnNumber(int turns) {
    gameState->setTurnNumber(turns);
}

/**
 * @brief Retrieves the number of moves made without a capture or pawn move.
 * @return The number of moves made without a capture or pawn move as an integer.
 */
int GameController::getNoCaptureOrPawnMoveCounter() const {
    return gameState->getNoCaptureOrPawnMoveCounter();
}

/**
 * @brief Sets the number of moves made without a capture or pawn move.
 * @param count The number of moves made without a capture or pawn move as an integer.
 */
void GameController::setNoCaptureOrPawnMoveCounter(int count) {
    gameState->setNoCaptureOrPawnMoveCounter(count);
}

/**
 * @brief Retrieves the current game status.
 * @return The current game status as a GameStatus enum.
 */
GameStatus GameController::getGameStatus() const {
    return gameState->getGameStatus();
}

/**
 * @brief Sets the current game status.
 * @param status The current game status as a GameStatus enum.
 */
void GameController::setGameStatus(GameStatus status) {
    gameState->setGameStatus(status);
}

/**
 * @brief Swaps the current player with the opponent player.
 */
void GameController::switchTurns() {
    gameState->switchTurns();
}

/**
 * @brief Validates if a move from the start position to the end position is legal according to the game rules.
 * @param start The start position of the piece on the board.
 * @param end The end position to move the piece to on the board.
 * @return true if the move is valid, false otherwise.
 */
bool GameController::validateMove(const Position& start, const Position& end) const {
    return gameState->getMutableBoard().isValidMove(start, end);
}

/**
 * @brief Moves chess piece from start position to end position if the move is legal according to game rules.
 * @param start The start position of the piece on the board.
 * @param end The end position to move the piece to on the board.
 * @return true if the move is executed, false otherwise.
 */
bool GameController::makeMove(const Position& start, const Position& end) {
    if (validateMove(start, end)) {
        // Make sure the move is in the vector of getPossibleMoves (makes sure you can't do a diff move when in check)
        std::vector<Position> possibleMoves = getPossibleMoves(start);
        if (std::find(possibleMoves.begin(), possibleMoves.end(), end) == possibleMoves.end()) {
            std::cout << "Invalid move" << std::endl;
            return false;
        }

        // keep track of most recent move in order to determine some unique draw scenarios later on 
        Piece* movingPiece = gameState->getMutableBoard().getPieceAtPosition(start);
        Piece* capturedPiece = gameState->getMutableBoard().getPieceAtPosition(end);

        PieceType movingPieceType = movingPiece->getPieceType();
        Colour movingPieceColour = movingPiece->getColour();
        PieceType capturedPieceType = capturedPiece ? capturedPiece->getPieceType() : PieceType::NONE;

        Move currentMove(start, end, movingPieceType, movingPieceColour, capturedPieceType);
        
        setLastMove(currentMove);
        addToGameHistory(currentMove);
        gameState->getMutableBoard().movePiece(start, end);

        // check if king has castled, move corresponding rook
        if(movingPieceType == PieceType::KING && abs(start.col - end.col) == 2) {
            int rookCol = end.col == 2 ? 0 : 7;
            int rookEndCol = end.col == 2 ? 3 : 5;
            Position rookStartPos = Position(end.row, rookCol);
            Position rookEndPos = Position(end.row, rookEndCol);
            gameState->getMutableBoard()[rookEndPos.row][rookEndPos.col] = std::move(gameState->getMutableBoard()[rookStartPos.row][rookStartPos.col]);
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->setPosition(rookEndPos);
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->setHasMoved();
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->updateValidMoves(gameState->getMutableBoard());
        }

        if(((movingPieceColour == Colour::WHITE && end.row == 0) 
            || (movingPieceColour == Colour::BLACK && end.row == 7))) {
                // allow pawn to be promoted first before switching turns 
                updateGameStatus();
                return true;
            }
    
         std::string whitePlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getCurrentPlayer()->getName() : getOpponentPlayer()->getName();

        std::string blackPlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getOpponentPlayer()->getName() : getCurrentPlayer()->getName();

        std::string pgn = generatePGN(getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, getGameStatus(), getBoardStatesMetadata().first);

        updateBoardStatesMetadata(pgn, getCurrentDate());
        switchTurns();
        // update valid moves for all other pieces based on the new board state 
        ChessBoard& boardMatrix = gameState->getMutableBoard();
        for(int row = 0; row < 8; ++row) {
            auto& inner = boardMatrix[row];
            for(auto& piece : inner) {
                if (piece != nullptr) {
                    piece->updateValidMoves(gameState->getMutableBoard());
                }
            }
        }
        updateGameStatus();
        return true;
    };
    std::cout << "Invalid move" << std::endl;
    return false;
}

/**
 * @brief Returns a list of possible moves for a piece at the given position.
 * @details This method checks the valid moves for the piece at the specified position.
 * @param position The position of the piece on the board.
 * @return A vector containing all possible moves for the piece at the given position.
 */
std::vector<Position> GameController::getPossibleMoves(const Position& position) const {
    Piece* piece = gameState->getMutableBoard().getPieceAtPosition(position);
    if (piece) {
        // If King is in check, only show moves that remove check 
        if (isKingInCheck()) {
            std::vector<Position> possibleMoves;
            for (const auto& move : piece->getValidMoves()) {
                if (tryMove(position, move)) {
                    possibleMoves.push_back(move);
                }
            }
            return possibleMoves;
        }
        /** If the piece is a King, check if a move will place it into check (i.e. an illegal move)
         * This check mainly pertains to when the posibility of the king capturing a piece will leave
         * it in check afterwards
        */
        else if (piece->getPieceType() == PieceType::KING) {
            std::vector<Position> possibleMoves;
            for (const auto& move : piece->getValidMoves()) {
                if (tryMove(position, move)) { /** If the king isn't in check after the move, 
                                                    then it is a valid move */
                    possibleMoves.push_back(move);
                }
            }
            return possibleMoves;
        }


        return piece->getValidMoves(); // Return all valid moves for the piece 
    }
    return {}; // Return an empty vector if there's no piece at the given position. 
}

/**
 * @brief Determines if the current game state is a stalemate.
 * @return true if the game is in a stalemate, false otherwise.
 */
bool GameController::isStalemate() const {
    if (isKingInCheck()) return false; // If the king is in check, it cannot be stalemate 

    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    // Loop through all the current player's pieces and see if any can make a valid move 
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(currentPlayerColour)) {
        auto possibleMoves = piece->getValidMoves();
        for (const auto& move : possibleMoves) {
            if (tryMove(piece->getPosition(), move)) {
                return false; // If any piece can make a valid move, it is not stalemate 
            }
        }
    }
    return true; // If no piece can make a valid move, it is stalemate 
}

/**
 * @brief Promotes a pawn to a different piece type.
 * @param position The position of the pawn on the board.
 * @param type The type of piece to promote the pawn to.
 * @return true if the pawn was successfully promoted, false otherwise.
 */
bool GameController::promotePawn(const Position& position, PieceType type) {
    // Validate Position 
    Piece* piece = gameState->getMutableBoard().getPieceAtPosition(position);
    if (!piece || piece->getPieceType() != PieceType::PAWN || piece->getColour() != gameState->getCurrentPlayer()->getColour()) {
        return false; // Invalid position or not a pawn of the current player 
    }

    // Validate Promotion 
    int promotionRow = piece->getColour() == Colour::WHITE ? 0 : 7;
    if (position.row != promotionRow) {
        std::cout << "The pawn is not in a position to be promoted" << std::endl;
        return false; // The pawn is not in a position to be promoted 
    }

    // Create New Piece and replace current piece with it 
    gameState->getMutableBoard()[position.row][position.col] = PieceFactory::createPiece(piece->getColour(), position, type);
    
    // Update Game Status 
    updateGameStatus();
    switchTurns();

    return true;
}

/**
 * @brief Determines if the king of the current player is in check.
 * @return true if the king of the current player is in check, false otherwise.
 */
bool GameController::isKingInCheck() const {
    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    Position kingPosition = gameState->getMutableBoard().getKingPosition(currentPlayerColour);

    // Loop through all the opponent's pieces and see if any can move to the king's position 
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(oppositeColour(currentPlayerColour))) {
        if (piece->isValidMove(piece->getPosition(), kingPosition)) {
            return true; // The king is in check if any opponent piece can move to the king's position 
        }
    }
    return false;
}

/**
 * @brief Gets the positions of the pieces involved in check
 * Used by UI to highlight the pieces involved in check
 * 
 * @return std::vector<Position> vector of positions of pieces involved
 */
std::vector<Position> GameController::getCheckPieces() const {
    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    Position kingPosition = gameState->getMutableBoard().getKingPosition(currentPlayerColour);
    std::vector<Position> checkPieces;
    checkPieces.push_back(kingPosition);

    // Loop through all the opponent's pieces and see if any can move to the king's position 
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(oppositeColour(currentPlayerColour))) {
        if (piece->isValidMove(piece->getPosition(), kingPosition)) {
            checkPieces.push_back(piece->getPosition());
        }
    }
    return checkPieces;
}

/**
 * @brief Tries to make a move on a copy of the current board and checks whether the move removes the check.
 * @param start The start position of the piece on the board.
 * @param end The end position to move the piece to on the board.
 * @return true if the move is valid and removes the check, false otherwise.
 */
bool GameController::tryMove(const Position& start, const Position& end) const {
    // Make a copy of the current board state 
    auto boardCopy = std::make_unique<ChessBoard>();
    boardCopy->initializeBoardFromString(gameState->getMutableBoard().getBoardStateAsString());

    // Try to make the move on the copy of the board 
    if (!boardCopy->movePiece(start, end)) return false; // If the move is not valid, return false 

    // Check if the king is still in check after the move 
    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    Position kingPosition = boardCopy->getKingPosition(currentPlayerColour);
    for (const auto& piece : boardCopy->getPiecesOfColour(oppositeColour(currentPlayerColour))) {
        piece->updateValidMoves(*boardCopy);
        if (piece->isValidMove(piece->getPosition(), kingPosition)) {
            return false; // The king is still in check after the move 
        }
    }
    return true; // The move removes the check 
}

/**
 * @brief Determines if the king of the current player is in checkmate.
 * @return true if the king of the current player is in checkmate, false otherwise.
 */
bool GameController::isKingInCheckmate() const {
    if (!isKingInCheck()) return false; // If the king is not in check, it cannot be in checkmate 

    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    // Loop through all the current player's pieces and see if any can make a valid move to remove the check 
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(currentPlayerColour)) {
        auto possibleMoves = piece->getValidMoves();
        for (const auto& move : possibleMoves) {
            if (tryMove(piece->getPosition(), move)) {
                return false; // If any piece can make a move to remove the check, it is not checkmate 
            }
        }
    }
    return true; // If no piece can make a valid move to remove the check, it is checkmate 
}

/**
 * @brief Checks the game state and updates the game status accordingly.
 */
void GameController::updateGameStatus() {
    if (isKingInCheckmate()) {
        gameState->setGameStatus(GameStatus::CHECKMATE);
        return; 
    } else if (isStalemate()) {
        gameState->setGameStatus(GameStatus::STALEMATE);
        return;
    } else if (isKingInCheck()) {
        gameState->setGameStatus(GameStatus::CHECK);
        // Notify the player that they are in check 
    } else if (isPlayerResigning()) {
        gameState->setGameStatus(GameStatus::RESIGN);
        return;
        // Possibly end the game and declare the winner 
    } else {
        gameState->setGameStatus(GameStatus::ONGOING);
    }

    // Threefold and Fivefold Repetition Draw Scenario 
    std::string currentState = gameState->getMutableBoard().getBoardStateAsString(); 

    // check if the length of the vector at the current state is 5 or more, indicating the same board state has occurred 5 or more times 
    if (gameState->getBoardStatesMetadata().second[currentState].size() >= 5) {
        // Fivefold repetition occurred, force a draw 
        gameState->setGameStatus(GameStatus::DRAW);
        return;
    } else if (gameState->getBoardStatesMetadata().second[currentState].size() >= 3) {
        // threefold repetition, prompt ui to ask players if they would like to end as a draw 
        gameState->setGameStatus(GameStatus::PROMPTDRAW);
    }

    // checks to see if a pawn has been moved or a piece has been captured in the last move 
    if (gameState->getLastMove().pieceCaptured == PieceType::NONE && gameState->getLastMove().pieceMoved != PieceType::PAWN) {
        int counter = gameState->getNoCaptureOrPawnMoveCounter();
        gameState->setNoCaptureOrPawnMoveCounter(++counter);
        if (counter >= 75) {
            // Seventy-five move rule occurred, automatically call game a draw 
            gameState->setGameStatus(GameStatus::DRAW);
            return;
        }
        else if (counter >= 50) {
            // Fifty-move rule occurred, prompt ui to ask players if they would like to end as a draw 
            gameState->setGameStatus(GameStatus::PROMPTDRAW);
            
            }
        }
    else {
        gameState->setNoCaptureOrPawnMoveCounter(0);
    }
    // checks to see if the board is in a dead position, if so, end the game as a draw 
    if (gameState->getMutableBoard().isDeadPosition()) {
        gameState->setGameStatus(GameStatus::DRAW);
        return;
    }
}

/**
 * @brief Retrieves the current game state. 
 * @return A pointer to the current GameState object.
 */
 
GameState* GameController::getGameState() const {
    return gameState.get();
}

/**
 * @brief Determines if the current player is resigning.
 * @return true if the current player is resigning, false otherwise.
 */
bool GameController::isPlayerResigning() const {
    return gameState->getCurrentPlayer()->getIsResigning();
}

/**
 * @brief Sets the current player as resigning once receiving prompt from UI.
 */
void GameController::setPlayerResigning() {
    return gameState->getCurrentPlayer()->setIsResigning();
}

/**
 * @brief Generates a list of moves from Stockfish based on the player's last move.
 * @return A vector containing all possible moves for the piece at the given position.
 */
std::vector<Move> GameController::generateStockfishMoves() {
    int stockfishSkillLevel;
    if (getStockfishDifficulty() == Difficulty::HARD) {
        stockfishSkillLevel = 20;
    }
    else if (getStockfishDifficulty() == Difficulty::MEDIUM) {
        stockfishSkillLevel = 10;
    }
    else if (getStockfishDifficulty() == Difficulty::EASY) {
        stockfishSkillLevel = 0;
    }

    // Generate the commands for Stockfish and write to a temporary script 
    std::ofstream script("temp_script.sh");
    script << "#!/bin/sh\n";
    script << "./stockfish-binary << EOF\n";
    script << "uci\n";
    script << "setoption name Skill Level value " << stockfishSkillLevel << "\n";  
    script << "position startpos moves " << getGameHistoryString() << "\n";
    script << "go movetime " << "5000" << "\n";  
    script << "quit\nEOF\n";
    script.close();

    // Set execute permissions for the script 
    system("chmod +x temp_script.sh");

    // Run the script and redirect the output to output.txt 
    system("./temp_script.sh > output.txt 2>&1");

    // Remove the temporary script 
    system("rm temp_script.sh");

    // Read the results from output.txt 
    std::ifstream infile("output.txt");
    std::string line;
    std::string stockfishOutput;
    while (std::getline(infile, line)) {
        stockfishOutput += line + "\n";
    }
    infile.close();

    // Parse Stockfish's output to create Move objects 
    std::vector<std::string> stockfishMoveStrings = parseStockfishOutput(stockfishOutput);
    std::vector<Move> stockfishMoves;
    for (const auto& moveString : stockfishMoveStrings) {
        stockfishMoves.emplace_back(parseMoveString(moveString));
    }

    return stockfishMoves;
}

/**
 * @brief Retrieves the game history in string format.
 * @return A string representation of the game history.
 */
std::string GameController::getGameHistoryString() const {
    return gameState->getGameHistoryString();
}

/**
 * @brief Sets the game history from a string representation.
 * @param gameHistoryString A string representation of the game history.
 */
void GameController::setGameHistoryString(const std::string& gameHistoryString) {
    return gameState->setGameHistoryString(gameHistoryString);
}

/**
 * @brief Converts a move string representation to a Move object.
 * @param moveString A string representation of a move.
 * @return A Move object corresponding to the parsed move string.
 */
Move GameController::parseMoveString(const std::string& moveString) const {
    return gameState->parseMoveString(moveString);
}

/**
 * @brief Retrieves the current difficulty setting of Stockfish.
 * @return The current difficulty setting.
 */
Difficulty GameController::getStockfishDifficulty() const {
    return stockfishDifficulty;
}

/**
 * @brief Updates the difficulty setting for Stockfish.
 * @param difficulty The desired difficulty setting.
 */
void GameController::setStockfishDifficulty(Difficulty difficulty) {
    stockfishDifficulty = difficulty;
}
