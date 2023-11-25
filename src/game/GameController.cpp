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
 * @author Bradley McGlynn, Chris Hosang, Riley Gavigan, Satvir Singh Uppal
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
GameController::GameController(std::unique_ptr<Player> whitePlayer, std::unique_ptr<Player> blackPlayer, std::string dbname, bool testMode) :
    gameState(std::make_unique<GameState>(std::move(whitePlayer), std::move(blackPlayer))) {
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
                        "CURRENTTIME REAL,"                          // Match timers, as doubles 
                        "OPPONENTTIME REAL,"
                        "GAMEHISTORY TEXT NOT NULL);"

                        "CREATE TABLE IF NOT EXISTS GAMEDATA("
                        "GAMEID INTEGER,"                       
                        "CURRENTUSER TEXT NOT NULL,"              // Usernames, as strings 
                        "OPPONENTUSER TEXT NOT NULL,"
                        "CURRENTCOLOUR TEXT NOT NULL,"                // Colours, as strings
                        "OPPONENTCOLOUR TEXT NOT NULL,"
                        "PGN TEXT NOT NULL,"
                        "TIMESTAMP TEXT NOT NULL,"
                        "USER1 TEXT NOT NULL,"
                        "USER2 TEXT NOT NULL);";          

    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr <<  "SQL Error: " << std::string(sqlite3_errmsg(db)) << std::endl;
        sqlite3_close(db);
    }

    // Initialize Game ID 
    gameID = generateGameID();
    this->testMode = testMode;
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
    gameState->resetGameState(testMode);

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

    auto stateData = getBoardStatesMetadata().first;
    // Don't allow saving an empty match that just started
    if (stateData.size() == 0) {
        return false;
    }

    int count = -1;
    if(sqlite3_prepare_v2(db, duplicateq.c_str(), -1, &duplicate_stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(duplicate_stmt, 1, gameID);
        if(sqlite3_step(duplicate_stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(duplicate_stmt, 0);
        }
        sqlite3_finalize(duplicate_stmt);
    }

    // If the game already exists, delete it to be replaced
    if(count > 0) {
        removeGame(gameID);
    }
    // Get users and colours
    std::string currentUser = getCurrentPlayer()->getUser().getUsername();
    std::string opponentUser = getOpponentPlayer()->getUser().getUsername();
    std::string currentColour = getCurrentPlayer()->getColour() == Colour::WHITE ? "White" : "Black";
    std::string opponentColour = getOpponentPlayer()->getColour() == Colour::WHITE ? "White" : "Black";

    // Get timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&now_c); 

    // Get user 1 and user 2
    std::string user1 = getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? currentUser : opponentUser;

    std::string user2 = getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? opponentUser : currentUser;

    // Get PGN for the game
    std::string pgn = PGNUtil::generatePGN(getCurrentPlayer()->getName(), user1, user2, getGameStatus(), getBoardStatesMetadata().first);

    // Query for GAMEDATA table
    std::string dataq = "INSERT INTO GAMEDATA(GAMEID, CURRENTUSER, OPPONENTUSER, CURRENTCOLOUR, OPPONENTCOLOUR, PGN, TIMESTAMP, USER1, USER2) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* data_stmt;

    if(sqlite3_prepare_v2(db, dataq.c_str(), -1, &data_stmt, 0) == SQLITE_OK) {
        // Add Game ID
        sqlite3_bind_int(data_stmt, 1, gameID);

        // Add Current User and Opponent User
        sqlite3_bind_text(data_stmt, 2, currentUser.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(data_stmt, 3, opponentUser.c_str(), -1, SQLITE_STATIC);

        // Add Current Colour and Opponent Colour
        sqlite3_bind_text(data_stmt, 4, currentColour.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(data_stmt, 5, opponentColour.c_str(), -1, SQLITE_STATIC);

        // Add PGN
        sqlite3_bind_text(data_stmt, 6, pgn.c_str(), -1, SQLITE_STATIC);

        // Add Timestamp of Game
        sqlite3_bind_text(data_stmt, 7, timestamp.c_str(), -1, SQLITE_STATIC);

        // Add User 1/2's Name For Tracking (User 1 is White)
        sqlite3_bind_text(data_stmt, 8, user1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(data_stmt, 9, user2.c_str(), -1, SQLITE_STATIC);

        // Finalize the statement
        if(sqlite3_step(data_stmt) == SQLITE_DONE) {
            sqlite3_finalize(data_stmt);
        }
    }

    // Query for GAMESTATE Table
    std::string q = "INSERT INTO GAMESTATE(GAMEID, BOARDSTATE, TURNNUMBER, GAMESTATUS, CURRENTTIME, OPPONENTTIME, GAMEHISTORY) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?);";

    // Get the GameStatus (ONGOING or CHECK)
    GameStatus status = stateData[stateData.size() - 1].gameStatus;
    std::string stringStatus = ConversionUtil::gameStatusToString(status);

    sqlite3_stmt* game_stmt;
    if(sqlite3_prepare_v2(db, q.c_str(), -1, &game_stmt, 0) == SQLITE_OK) {
        // Add Game ID
        sqlite3_bind_int(game_stmt, 1, gameID);

        // Add Board State String
        sqlite3_bind_text(game_stmt, 2, stateData[stateData.size() - 1].boardState.c_str(), -1, SQLITE_STATIC);

        // Add Turn Number
        sqlite3_bind_int(game_stmt, 3, stateData[stateData.size() - 1].turnNumber);

        // Add Game Status String
        sqlite3_bind_text(game_stmt, 4, stringStatus.c_str(), -1, SQLITE_STATIC);

        // Add Time Left          
        sqlite3_bind_double(game_stmt, 5, gameState->getCurrentPlayer()->getTimeLeft().count());
        sqlite3_bind_double(game_stmt, 6, gameState->getOpponentPlayer()->getTimeLeft().count());      
     
        // Add Game History String
        std::string gameHistory = getGameHistoryString();
        sqlite3_bind_text(game_stmt, 7, gameHistory.c_str(), -1, SQLITE_STATIC);

        // Finalize statement
        if(sqlite3_step(game_stmt) == SQLITE_DONE) {
            sqlite3_finalize(game_stmt);
        }
    }
    return true;
}
int GameController::getNumSaves(User* user1, User* user2){
    // Get usernames of both users
    std::string username1 = user1->getUsername();
    std::string username2 = user2->getUsername();

    std::string savesq = "SELECT COUNT(GAMEID) FROM GAMEDATA WHERE USER1 = ? AND USER2 = ?;";
    sqlite3_stmt* count_stmt;

    int count = 0;
    if(sqlite3_prepare_v2(db, savesq.c_str(), -1, &count_stmt, 0) == SQLITE_OK) {
        // Bind username1 and username2
        sqlite3_bind_text(count_stmt, 1, username1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(count_stmt, 2, username2.c_str(), -1, SQLITE_STATIC);

        if(sqlite3_step(count_stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(count_stmt, 0);
        }
        sqlite3_finalize(count_stmt);
    }
    return count;
}

/**
 * @brief Gets all of the loadable game IDs for 2 users
 * 
 * @param user1 the first user
 * @param user2 the second user
 * @return std::vector<int, std::vector<std::string>> a vector of game IDs
 * The vector of strings contains: Current Player, Opponent Player, Current Colour, Opponent Colour, Timestamp
 */
std::vector<std::unordered_map<std::string, std::string>> GameController::getLoadableGames(User* user1, User* user2) {
    std::vector<std::unordered_map<std::string, std::string>> loadableGames;

    // Get usernames of both users
    std::string username1 = user1->getUsername();
    std::string username2 = user2->getUsername();

    std::string savesq = "SELECT GAMEID, CURRENTUSER, OPPONENTUSER, CURRENTCOLOUR, OPPONENTCOLOUR, TIMESTAMP FROM GAMEDATA WHERE USER1 = ? AND USER2 = ?;";
    sqlite3_stmt* game_stmt;

    if(sqlite3_prepare_v2(db, savesq.c_str(), -1, &game_stmt, 0) == SQLITE_OK) {
        // Bind username1 and username2
        sqlite3_bind_text(game_stmt, 1, username1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(game_stmt, 2, username2.c_str(), -1, SQLITE_STATIC);

        while(sqlite3_step(game_stmt) == SQLITE_ROW) {
            std::unordered_map<std::string, std::string> gameData;
            int gameID = sqlite3_column_int(game_stmt, 0);

            gameData["gameID"] = std::to_string(gameID);
            gameData["currentUser"] = reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 1));
            gameData["opponentUser"] = reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 2));
            gameData["currentColour"] = reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 3));
            gameData["opponentColour"] = reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 4));
            gameData["timestamp"] = std::to_string(sqlite3_column_double(game_stmt, 5));

            std::string savesq2 = "SELECT TURNNUMBER, GAMESTATUS, CURRENTTIME, OPPONENTTIME FROM GAMESTATE WHERE GAMEID = ?;";
            sqlite3_stmt* game_stmt2;

            if(sqlite3_prepare_v2(db, savesq2.c_str(), -1, &game_stmt2, 0) == SQLITE_OK) {
                // Bind game ID
                sqlite3_bind_int(game_stmt2, 1, gameID);

                while(sqlite3_step(game_stmt2) == SQLITE_ROW) {
                    gameData["turnNumber"] = std::to_string(sqlite3_column_int(game_stmt2, 0));
                    gameData["gameStatus"] = reinterpret_cast<const char*>(sqlite3_column_text(game_stmt2, 1));
                    gameData["currentTime"] = std::to_string(sqlite3_column_double(game_stmt2, 2));
                    gameData["opponentTime"] = std::to_string(sqlite3_column_double(game_stmt2, 3));
                }
                sqlite3_finalize(game_stmt2);
            }

            loadableGames.push_back(gameData);
        }
        sqlite3_finalize(game_stmt);
    }

    return loadableGames;
}


/**
 * @brief Loads a game state from a file.
 * @param gameID ID of the game to be loaded.
 * @return true if the game was successfully loaded, false otherwise.
 */
bool GameController::loadGame(int gameID, User* user1, User* user2) {
    // Select from GAMEDATA table
    std::string dataq = "SELECT CURRENTUSER, OPPONENTUSER, CURRENTCOLOUR, OPPONENTCOLOUR, PGN, TIMESTAMP, USER1, USER2 FROM GAMEDATA WHERE GAMEID = ?;";
    sqlite3_stmt* data_stmt;

    if(sqlite3_prepare_v2(db, dataq.c_str(), -1, &data_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(data_stmt, 1, gameID);

    gameState->resetGameState();

    int step = sqlite3_step(data_stmt); 
    if(step == SQLITE_ROW) {
        // Get Current User and Opponent User
        std::string currentUsername(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 0)));
        std::string opponentUsername(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 1)));
        std::string currentColourString(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 2)));
        std::string opponentColourString(reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 3)));
        Colour currentColour, opponentColour;
        if (currentColourString == "White") {
            currentColour = Colour::WHITE;
            opponentColour = Colour::BLACK;
        } else {
            currentColour = Colour::BLACK;
            opponentColour = Colour::WHITE;
        }

        // Get PGN for the game
        std::string pgn = reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 4));

        // Get timestamp for the game
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(data_stmt, 5));

        User* currentUser = currentUsername == user1->getUsername() ? user1 : user2;
        User* opponentUser = opponentUsername == user1->getUsername() ? user1 : user2;
        
        // Initialize players with initial timers of 0 minutes
        setCurrentPlayer(std::make_unique<Player>(*currentUser, currentColour, std::chrono::minutes(0)));
        setOpponentPlayer(std::make_unique<Player>(*opponentUser, opponentColour, std::chrono::minutes(0)));
    } 
    else {
        std::cerr << "Failed to fetch data from GAMEDATA: " << step << std::endl;
        sqlite3_finalize(data_stmt);
        return false;     
    }
    sqlite3_finalize(data_stmt);

    // Select from GAMESTATE table
    std::string stateq = "SELECT BOARDSTATE, TURNNUMBER, GAMESTATUS, CURRENTTIME, OPPONENTTIME, GAMEHISTORY FROM GAMESTATE WHERE GAMEID = ? ORDER BY TURNNUMBER;";
    sqlite3_stmt* game_stmt;

    if(sqlite3_prepare_v2(db, stateq.c_str(), -1, &game_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(game_stmt, 1, gameID);

    // Replace current game with saved game states 
    while(sqlite3_step(game_stmt) == SQLITE_ROW) {
        // Initialize GameState Chess Board from BOARDSTATE
        getMutableBoard().initializeBoardFromString(reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 0)));

        // Set turn number to TURNNUMBER
        setTurnNumber(sqlite3_column_int(game_stmt, 1));

        // Get Game Status and set it from GAMESTATUS
        std::string status(reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 2))); 
        setGameStatus(ConversionUtil::stringToGameStatus(status));

        // Get Current Player Time and Opponent Player Time and set them
        getCurrentPlayer()->setTimeLeft(std::chrono::duration<double>(sqlite3_column_double(game_stmt, 3)));
        getOpponentPlayer()->setTimeLeft(std::chrono::duration<double>(sqlite3_column_double(game_stmt, 4)));

        // Get game history string
        std::string gameHistory(reinterpret_cast<const char*>(sqlite3_column_text(game_stmt, 5)));
        setGameHistoryString(gameHistory);

        // Update board state metadata
        std::string pgn = PGNUtil::generatePGN(getCurrentPlayer()->getName(), user1->getUsername(), user2->getUsername(), getGameStatus(), getBoardStatesMetadata().first);
        updateBoardStatesMetadata(pgn, PGNUtil::getCurrentDate());
    }
    sqlite3_finalize(game_stmt);

    std::cout << "Loaded Game History: " << getGameHistoryString() << std::endl;

    // Update the Player's User
    getCurrentPlayer()->getUser().setUsername(getCurrentPlayer()->getName());
    getOpponentPlayer()->getUser().setUsername(getOpponentPlayer()->getName());

    this->gameID = gameID;
    return true;
}

/**
 * @brief Removes a game from the database with gameID provided
 * 
 * @return true if removed successfully
 * @return false if not removed
 */
bool GameController::removeGame(int gameID) {
    // Remove game data from GAMEDATA
    std::string dataq = "DELETE FROM GAMEDATA WHERE GAMEID = ?;";
    sqlite3_stmt* data_stmt;

    if(sqlite3_prepare_v2(db, dataq.c_str(), -1, &data_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(data_stmt, 1, gameID);

    int step = sqlite3_step(data_stmt); 
    if(step != SQLITE_DONE) {
        std::cerr << "Failed to remove data from GAMEDATA: " << step << std::endl;
        sqlite3_finalize(data_stmt);
        return false;     
    }
    sqlite3_finalize(data_stmt);

    // Remove game states from GAMESTATE
    std::string stateq = "DELETE FROM GAMESTATE WHERE GAMEID = ?;";
    sqlite3_stmt* game_stmt;

    if(sqlite3_prepare_v2(db, stateq.c_str(), -1, &game_stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(game_stmt, 1, gameID);

    step = sqlite3_step(game_stmt);
    if(step != SQLITE_DONE) {
        std::cerr << "Failed to remove data from GAMESTATE: " << step << std::endl;
        sqlite3_finalize(game_stmt);
        return false;     
    }
    sqlite3_finalize(game_stmt);

    std::cout << "Game ID " << gameID << " removed." << std::endl;
    return true;
}

/**
 * @brief End the game and update the user statistics.
 */
void GameController::endGame() {
    if(testMode) {
        return;
    }

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

        // Update both user's Elo ratings
        double winnerElo = winner->getUser().getElo();
        double loserElo = loser->getUser().getElo();

        winner->getUser().updateElo(getEloChange(winnerElo, loserElo, 1.0));
        loser->getUser().updateElo(getEloChange(loserElo, winnerElo, 0.0));
    }
    // Create UserManager instance 
    UserManager userManager = UserManager("db/default.sql");
    
    // Update databases for user stats 
    if (getCurrentPlayer()->getUser().getUsername() != "") {
        userManager.updateUserStats(getCurrentPlayer()->getUser().getUsername(), getCurrentPlayer()->getUser().getWins(), getCurrentPlayer()->getUser().getLosses(), getCurrentPlayer()->getUser().getDraws(), getCurrentPlayer()->getUser().getElo());
    }
    if (getOpponentPlayer()->getUser().getUsername() != "") {
        userManager.updateUserStats(getOpponentPlayer()->getUser().getUsername(), getOpponentPlayer()->getUser().getWins(), getOpponentPlayer()->getUser().getLosses(), getOpponentPlayer()->getUser().getDraws(), getOpponentPlayer()->getUser().getElo());
    }

    // Remove the game from the database if it exists
    removeGame(gameID);
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
    gameState->setGameStatus(getGameStatus());
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
    if (testMode || validateMove(start, end)) {
        // Make sure the move is in the vector of getPossibleMoves (makes sure you can't do a diff move when in check)
        std::vector<Position> possibleMoves = getPossibleMoves(start);
        if (!testMode && std::find(possibleMoves.begin(), possibleMoves.end(), end) == possibleMoves.end()) {
            std::cout << "Invalid move" << std::endl;
            return false;
        }

        // keep track of most recent move in order to determine some unique draw scenarios later on 
        Piece* movingPiece = gameState->getMutableBoard().getPieceAtPosition(start);
        Piece* capturedPiece = gameState->getMutableBoard().getPieceAtPosition(end);

        if(capturedPiece && capturedPiece->getPieceType() == PieceType::KING) {
            std::cout << "Invalid move" << std::endl;
            return false;
        } 

        PieceType movingPieceType = movingPiece->getPieceType();
        Colour movingPieceColour = movingPiece->getColour();
        PieceType capturedPieceType = capturedPiece ? capturedPiece->getPieceType() : PieceType::NONE;

        Move currentMove(start, end, movingPieceType, movingPieceColour, capturedPieceType);

        // Print out the move
        std::cout << "Move: " << ConversionUtil::turnMoveIntoString(currentMove) << std::endl;
        
        setLastMove(currentMove);
        addToGameHistory(currentMove);
        gameState->getMutableBoard().movePiece(start, end, testMode);

        // check if king has castled, move corresponding rook
        if(!testMode && movingPieceType == PieceType::KING && abs(start.col - end.col) == 2) {
            int rookCol = end.col == 2 ? 0 : 7;
            int rookEndCol = end.col == 2 ? 3 : 5;
            Position rookStartPos = Position(end.row, rookCol);
            Position rookEndPos = Position(end.row, rookEndCol);
            gameState->getMutableBoard()[rookEndPos.row][rookEndPos.col] = std::move(gameState->getMutableBoard()[rookStartPos.row][rookStartPos.col]);
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->setPosition(rookEndPos);
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->setHasMoved();
            gameState->getMutableBoard().getPieceAtPosition(rookEndPos)->updateValidMoves(gameState->getMutableBoard());
        }

        if(!testMode && movingPieceType == PieceType::PAWN && ((movingPieceColour == Colour::WHITE && end.row == 0) 
            || (movingPieceColour == Colour::BLACK && end.row == 7))) {
                // allow pawn to be promoted first before switching turns 
                updateGameStatus();
                return true;
            }
        
        // En passant 
        if (movingPieceType == PieceType::PAWN && capturedPiece == nullptr && start.col != end.col) {
            int capturedPieceRow = movingPieceColour == Colour::WHITE ? end.row + 1 : end.row - 1;
            Position capturedPiecePosition{capturedPieceRow, end.col};
            gameState->getMutableBoard().placePiece(capturedPiecePosition, nullptr);
        }
    
        std::string whitePlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getCurrentPlayer()->getName() : getOpponentPlayer()->getName();

        std::string blackPlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getOpponentPlayer()->getName() : getCurrentPlayer()->getName();

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
        switchTurns();
        updateGameStatus();

        std::string pgn = PGNUtil::generatePGN(getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, getGameStatus(), getBoardStatesMetadata().first);
        updateBoardStatesMetadata(pgn, PGNUtil::getCurrentDate());

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
        piece->updateValidMoves(gameState->getMutableBoard());
        // If King is in check, only show moves that remove check 
        if (isKingInCheck()) {
            std::vector<Position> possibleMoves;
            for (const auto& move : piece->getValidMoves()) {
                if (testMode || tryMove(position, move)) {
                    possibleMoves.push_back(move);
                }
            }
            return possibleMoves;
        }
        // Check if a move will place the king into check (i.e. an illegal move)
        else if (!isKingInCheck()) {
            std::vector<Position> possibleMoves;
            for (const auto& move : piece->getValidMoves()) {
                if (testMode || tryMove(position, move)) { // If the king isn't in check after the move, 
                                                    //then it is a valid move 
                    possibleMoves.push_back(move);
                }
            }
            return possibleMoves;
        }

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
    
    std::string whitePlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                            ? getCurrentPlayer()->getName() : getOpponentPlayer()->getName();

    std::string blackPlayerName = getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? getOpponentPlayer()->getName() : getCurrentPlayer()->getName();

    std::string pgn = PGNUtil::generatePGN(getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, getGameStatus(), getBoardStatesMetadata().first);

    updateBoardStatesMetadata(pgn, PGNUtil::getCurrentDate());

    // Update valid moves and board status
    switchTurns();

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
}

/**
 * @brief Determines if the king of the current player is in check.
 * @return true if the king of the current player is in check, false otherwise.
 */
bool GameController::isKingInCheck() const {
    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    Position kingPosition = gameState->getMutableBoard().getKingPosition(currentPlayerColour);

    // Loop through all the opponent's pieces and see if any can move to the king's position 
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(ConversionUtil::oppositeColour(currentPlayerColour))) {
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
    for (const auto& piece : gameState->getMutableBoard().getPiecesOfColour(ConversionUtil::oppositeColour(currentPlayerColour))) {
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

    // King cannot castle out of check, ensure that the king is not castling
    if (isKingInCheck() && boardCopy->getPieceAtPosition(start)->getPieceType() == PieceType::KING && abs(start.col - end.col) == 2) {
        return false;
    }

    // Try to make the move on the copy of the board 
    if (!boardCopy->movePiece(start, end)) return false; // If the move is not valid, return false 
    // Check if the king is still in check after the move 
    Colour currentPlayerColour = gameState->getCurrentPlayer()->getColour();
    Position kingPosition = boardCopy->getKingPosition(currentPlayerColour);
    for (const auto& piece : boardCopy->getPiecesOfColour(ConversionUtil::oppositeColour(currentPlayerColour))) {
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
 * @brief Gets the current test mode.
 * @return Test mode.
*/
bool GameController::getTestMode() {
    return testMode;
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
 * @brief Calculates the probability of the current player winning. 
 * @param currentElo Current player's Elo.
 * @param opponentElo Opponent player's Elo.
 * @return Probability of current player winning, in the range [0, 1].
*/
double GameController::chanceOfWinning(double currentElo, double opponentElo) {
    // For more information behind this algorithm's intuition: https://mattmazzola.medium.com/understanding-the-elo-rating-system-264572c7a2b4
    return (1.0 / (1.0 + pow(10, ((opponentElo - currentElo) / 400))));
}

/**
 * @brief Calculates the change in a player's Elo based on if they won or lost.
 * @param currentElo Current player's Elo.
 * @param opponentElo Opponent player's Elo.
 * @param score 1.0 if the current player won, 0.0 if the opponent player won.
 * @returns Change in Elo.
*/
double GameController::getEloChange(double currentElo, double opponentElo, double score) {
    return eloConstant * (score - chanceOfWinning(currentElo, opponentElo));
}


/**
 * @brief Calculates the change in a player's Elo based on if they won or lost.
 * @param currentElo Current player's Elo.
 * @param opponentElo Opponent player's Elo.
 * @returns Pair of Elo ratings: (change if win, change if lose).
*/
std::pair<double, double> GameController::getEloChange(double currentElo, double opponentElo) {
    return std::make_pair<double, double>(eloConstant * (1.0 - chanceOfWinning(currentElo, opponentElo)), eloConstant * (0.0 - chanceOfWinning(currentElo, opponentElo)));
}