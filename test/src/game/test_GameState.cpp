/**
 * @file test_GameState.cpp
 * @brief Unit tests for the GameState class.
 *
 * This file contains unit tests for the GameState class, including the constructor, resetGameState, getMutableBoard, getCurrentPlayer, getOpponentPlayer, getBoardStatesMetadata, getRemainingTime, isTimeOut, decrementPlayerTime, getGameHistory, getWinner, getTurnStartTime, getLastMove, getTurnNumber, getNoCaptureOrPawnMoveCounter, getGameStatus, switchTurns, setLastMove, setNoCaptureOrPawnMoveCounter, setGameStatus, setTurnStartTime, and setCurrentPlayer methods.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "GameState.h"
#include "Piece.h"
#include "Player.h"
#include "User.h"

/**
 * @brief Test fixture for the GameState class.
 */
class GameStateTests : public testing::Test {
protected:
    GameState* gs;

    // Set up game state 
    void SetUp() override {
        User user1("Alice");
        User user2("Bob");
        std::unique_ptr<Player> player1 = std::make_unique<Player>(user1, Colour::WHITE, std::chrono::minutes(5));
        std::unique_ptr<Player> player2 = std::make_unique<Player>(user2, Colour::BLACK, std::chrono::minutes(5));

        gs = new GameState(std::move(player1), std::move(player2));
    }

    // Delete game state 
    void TearDown() override {
        delete gs;
    };
};

TEST_F(GameStateTests, TestConstructor) {
    ChessBoard& board = gs->getMutableBoard();

    // Test if the players are set correctly
    EXPECT_EQ(gs->getCurrentPlayer()->getName(), "Alice");
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "Bob");

    // Test if the last move is initialized correctly
    Move lastMove = gs->getLastMove();
    Position pos(0,0);
    EXPECT_EQ(lastMove.start, pos);
    EXPECT_EQ(lastMove.end, pos);
    EXPECT_EQ(lastMove.pieceMoved, PieceType::NONE);
    EXPECT_EQ(lastMove.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(lastMove.playerColour, Colour::WHITE);

    // Test if the noCaptureOrPawnMoveCounter is initialized correctly
    EXPECT_EQ(gs->getNoCaptureOrPawnMoveCounter(), 0);

    // Test if the turn start time is initialized correctly
    auto now = std::chrono::steady_clock::now();
    auto diff = now - gs->getTurnStartTime();
    EXPECT_LT(diff, std::chrono::milliseconds(100));  // Assuming the difference should be less than 100 milliseconds

    // Test if the game status is initialized correctly
    EXPECT_EQ(gs->getGameStatus(), GameStatus::ONGOING);
}

TEST_F(GameStateTests, TestResetGameState) {
    ChessBoard& board = gs->getMutableBoard();

    // Make some moves to the board
    board.movePiece(Position{1, 0}, Position{3, 0});
    board.movePiece(Position{6, 0}, Position{4, 0});
    board.movePiece(Position{0, 1}, Position{2, 0});
    board.movePiece(Position{7, 1}, Position{5, 0});

    // Set some values to the game state
    gs->setLastMove(Move(Position{7, 1}, Position{5, 0}, PieceType::KNIGHT, Colour::WHITE));
    gs->setNoCaptureOrPawnMoveCounter(10);
    gs->setGameStatus(GameStatus::STALEMATE);

    // Reset the game state
    gs->resetGameState();

    // Test if the board is reset correctly
    ChessBoard& resetBoard = gs->getMutableBoard();
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{1, 0})->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{2, 0}), nullptr);
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{6, 0})->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{5, 0}), nullptr);
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{0, 1})->getPieceType(), PieceType::KNIGHT);
    EXPECT_EQ(resetBoard.getPieceAtPosition(Position{7, 1})->getPieceType(), PieceType::KNIGHT);

    // Test if the players are reset correctly
    EXPECT_EQ(gs->getCurrentPlayer(), nullptr);
    EXPECT_EQ(gs->getOpponentPlayer(), nullptr);

    // Test if the last move is reset correctly
    Move lastMove = gs->getLastMove();
    Position pos(0,0);
    EXPECT_EQ(lastMove.start, pos);
    EXPECT_EQ(lastMove.end, pos);
    EXPECT_EQ(lastMove.pieceMoved, PieceType::NONE);
    EXPECT_EQ(lastMove.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(lastMove.playerColour, Colour::WHITE);

    // Test if the noCaptureOrPawnMoveCounter is reset correctly
    EXPECT_EQ(gs->getNoCaptureOrPawnMoveCounter(), 0);

    // Test if the turn start time is reset correctly
    auto now = std::chrono::steady_clock::now();
    auto diff = now - gs->getTurnStartTime();
    EXPECT_LT(diff, std::chrono::milliseconds(100));  // Assuming the difference should be less than 100 milliseconds

    // Test if the game status is reset correctly
    EXPECT_EQ(gs->getGameStatus(), GameStatus::ONGOING);
}

TEST_F(GameStateTests, TestGetMutableBoard) {
    EXPECT_EQ(gs->getMutableBoard().getPieceAtPosition(Position{0, 0})->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(gs->getMutableBoard().getPieceAtPosition(Position{0, 4})->getPieceType(), PieceType::KING);
    EXPECT_EQ(gs->getMutableBoard().getPieceAtPosition(Position{7, 4})->getPieceType(), PieceType::KING);
}

TEST_F(GameStateTests, TestCurrentPlayer) {
    User user3("newAlice");
    User user4("newBob");
    std::unique_ptr<Player> player3 = std::make_unique<Player>(user3, Colour::WHITE, std::chrono::minutes(5));
    std::unique_ptr<Player> player4 = std::make_unique<Player>(user4, Colour::BLACK, std::chrono::minutes(5));

    EXPECT_EQ(gs->getCurrentPlayer()->getName(), "Alice");
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "Bob");

    gs->setCurrentPlayer(std::move(player3));
    gs->setOpponentPlayer(std::move(player4));

    EXPECT_EQ(gs->getCurrentPlayer()->getName(), "newAlice");
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "newBob");
}

TEST_F(GameStateTests, TestOpponentPlayer) {
    User user4("newBob");
    std::unique_ptr<Player> player4 = std::make_unique<Player>(user4, Colour::BLACK, std::chrono::minutes(5));
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "Bob");

    gs->setOpponentPlayer(std::move(player4));

    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "newBob");
}

TEST_F(GameStateTests, TestBoardStatesMetadata) {
    EXPECT_EQ(gs->getBoardStatesMetadata().first.size(), 0);

    // Update board state metadata 
    std::string whitePlayerName = gs->getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? gs->getCurrentPlayer()->getName() : gs->getOpponentPlayer()->getName();

    std::string blackPlayerName = gs->getCurrentPlayer()->getColour() == Colour::BLACK 
                        ? gs->getCurrentPlayer()->getName() : gs->getOpponentPlayer()->getName();

    GameStatus gameStatus = gs->getGameStatus();

    std::string matchResult;
    if (gameStatus == GameStatus::CHECKMATE || gameStatus == GameStatus::RESIGN) {
        matchResult =  gs->getCurrentPlayer()->getColour() == Colour::BLACK ? "1-0" : "0-1";
    } else if (gameStatus == GameStatus::DRAW || gameStatus == GameStatus::STALEMATE) {
        matchResult = "1/2-1/2";
    } else {
        matchResult = "*";  // Uncertain outcome or game still ongoing
    }

    std::string eventName = "Local Match: " + whitePlayerName + " vs. " + blackPlayerName;
    std::string site = "CS 3307";
    std::string date = "2023.10.27";  // Ensure this method outputs the date in "YYYY.MM.DD" format
    std::string round = "1";

    std::string pgn = generatePGN(eventName, site, date, round, whitePlayerName, blackPlayerName, matchResult, gs->getBoardStatesMetadata().first);
    std::cout << "1 \n " << pgn << std::endl;

    gs->updateBoardStatesMetadata(pgn, date);
    BoardMetadata initialMetadata = gs->getBoardStatesMetadata().first[0];
    
    EXPECT_EQ(gs->getBoardStatesMetadata().first.size(), 1);
    EXPECT_EQ(initialMetadata.boardState.compare(
        "r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\n........\n........\nP-P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n"), 0);
    EXPECT_EQ(initialMetadata.currentPlayer->getName().compare("Alice"), 0);
    EXPECT_EQ(initialMetadata.opponentPlayer->getName().compare("Bob"), 0);
    EXPECT_EQ(initialMetadata.currentPlayerColour, Colour::WHITE);
    EXPECT_EQ(initialMetadata.opponentPlayerColour, Colour::BLACK);
    EXPECT_EQ(initialMetadata.turnNumber, 1);
    EXPECT_EQ(initialMetadata.gameStatus, GameStatus::ONGOING);
    EXPECT_EQ(initialMetadata.moveMade.start, Position(0, 0));
    EXPECT_EQ(initialMetadata.moveMade.end, Position(0, 0));
    EXPECT_EQ(initialMetadata.moveMade.pieceMoved, PieceType::NONE);
    EXPECT_EQ(initialMetadata.moveMade.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(initialMetadata.moveMade.playerColour, Colour::WHITE); 
    EXPECT_EQ(gs->getBoardStatesMetadata().second[gs->getMutableBoard().getBoardStateAsString()].size(), 1);
    
    gs->getMutableBoard().movePiece(Position{6, 0}, Position{4, 0});
    gs->switchTurns();
    gs->setLastMove(Move(Position{6, 0}, Position{4, 0}, PieceType::PAWN, Colour::WHITE));

    gameStatus = gs->getGameStatus();
    if (gameStatus == GameStatus::CHECKMATE || gameStatus == GameStatus::RESIGN) {
        matchResult =  gs->getCurrentPlayer()->getColour() == Colour::BLACK ? "1-0" : "0-1";
    } else if (gameStatus == GameStatus::DRAW || gameStatus == GameStatus::STALEMATE) {
        matchResult = "1/2-1/2";
    } else {
        matchResult = "*";  // Uncertain outcome or game still ongoing
    }

    pgn = generatePGN(eventName, site, date, round, whitePlayerName, blackPlayerName, matchResult, gs->getBoardStatesMetadata().first);
    std::cout << "2 \n " << pgn << std::endl;

    gs->updateBoardStatesMetadata(pgn, date);
    BoardMetadata updatedMetadata = gs->getBoardStatesMetadata().first[1];

    EXPECT_EQ(gs->getBoardStatesMetadata().first.size(), 2);
    EXPECT_EQ(updatedMetadata.boardState.compare(
        "r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\nP-.......\n........\n.P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n"), 0);
    EXPECT_EQ(updatedMetadata.currentPlayer->getName().compare("Bob"), 0);
    EXPECT_EQ(updatedMetadata.opponentPlayer->getName().compare("Alice"), 0);
    EXPECT_EQ(updatedMetadata.currentPlayerColour, Colour::BLACK);
    EXPECT_EQ(updatedMetadata.opponentPlayerColour, Colour::WHITE);
    EXPECT_EQ(updatedMetadata.turnNumber, 1);
    EXPECT_EQ(updatedMetadata.gameStatus, GameStatus::ONGOING);
    EXPECT_EQ(updatedMetadata.moveMade.start, Position(6, 0));
    EXPECT_EQ(updatedMetadata.moveMade.end, Position(4, 0));
    EXPECT_EQ(updatedMetadata.moveMade.pieceMoved, PieceType::PAWN);
    EXPECT_EQ(updatedMetadata.moveMade.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(updatedMetadata.moveMade.playerColour, Colour::WHITE);
    EXPECT_EQ(gs->getBoardStatesMetadata().second[gs->getMutableBoard().getBoardStateAsString()].size(), 1);

    gs->getMutableBoard().initializeBoard(); // reset board to see a copy in the metadata (i.e see a repeated board state)

    gameStatus = gs->getGameStatus();
    if (gameStatus == GameStatus::CHECKMATE || gameStatus == GameStatus::RESIGN) {
        matchResult =  gs->getCurrentPlayer()->getColour() == Colour::BLACK ? "1-0" : "0-1";
    } else if (gameStatus == GameStatus::DRAW || gameStatus == GameStatus::STALEMATE) {
        matchResult = "1/2-1/2";
    } else {
        matchResult = "*";  // Uncertain outcome or game still ongoing
    }

    pgn = generatePGN(eventName, site, date, round, whitePlayerName, blackPlayerName, matchResult, gs->getBoardStatesMetadata().first);

    std::cout << "3 \n " << pgn << std::endl;

    gs->updateBoardStatesMetadata(pgn, date);
    gs->updateBoardStatesMetadata(pgn, date);
    gs->updateBoardStatesMetadata(pgn, date);
    gs->updateBoardStatesMetadata(pgn, date);

    EXPECT_EQ(gs->getBoardStatesMetadata().second[gs->getMutableBoard().getBoardStateAsString()].size(), 5);
}


TEST_F(GameStateTests, PlayerRemainingTime) {
    // Test initial remaining time
    EXPECT_EQ(gs->getRemainingTime(), std::chrono::minutes(5));
    EXPECT_FALSE(gs->isTimeOut());

    // Decrement player time
    gs->decrementPlayerTime(std::chrono::duration<double>(30));

    // Check if time is decremented correctly
    double timeLeft = std::chrono::duration<double>(gs->getCurrentPlayer()->getTimeLeft()).count();
    EXPECT_LT(timeLeft, std::chrono::duration<double>(std::chrono::minutes(5)).count());
    EXPECT_FALSE(gs->isTimeOut());

    // Switch turns
    gs->switchTurns();

    // Check if remaining time is still correct
    EXPECT_EQ(gs->getRemainingTime(), std::chrono::minutes(5));

    // Timeout test
    gs->decrementPlayerTime(std::chrono::duration<double>(700));
    EXPECT_TRUE(gs->isTimeOut());
}

TEST_F(GameStateTests, GameHistory) {
    // Test initial game history
    EXPECT_EQ(gs->getGameHistory().size(), 0);
    EXPECT_EQ(gs->getGameHistoryString(), "");

    Move move1(Position{7, 1}, Position{5, 0}, PieceType::KNIGHT, Colour::WHITE);
    Move move2(Position{1, 1}, Position{3, 1}, PieceType::PAWN, Colour::BLACK);

    // Add moves to game history
    gs->addToGameHistory(move1);
    gs->addToGameHistory(move2);

    // Check if moves are added correctly
    EXPECT_EQ(gs->getGameHistory().size(), 2);
    EXPECT_EQ(gs->getGameHistory()[0].start, Position(7, 1));
    EXPECT_EQ(gs->getGameHistory()[0].end, Position(5, 0));
    EXPECT_EQ(gs->getGameHistory()[0].pieceMoved, PieceType::KNIGHT);
    EXPECT_EQ(gs->getGameHistory()[0].pieceCaptured, PieceType::NONE);
    EXPECT_EQ(gs->getGameHistory()[0].playerColour, Colour::WHITE);

    EXPECT_EQ(gs->getGameHistory()[1].start, Position(1, 1));
    EXPECT_EQ(gs->getGameHistory()[1].end, Position(3, 1));
    EXPECT_EQ(gs->getGameHistory()[1].pieceMoved, PieceType::PAWN);
    EXPECT_EQ(gs->getGameHistory()[1].pieceCaptured, PieceType::NONE);
    EXPECT_EQ(gs->getGameHistory()[1].playerColour, Colour::BLACK);

    EXPECT_EQ(gs->getGameHistoryString(), "b1a3 b7b5 ");
    gs->setGameHistoryString("a1a2 b7b5 ");
    EXPECT_EQ(gs->getGameHistoryString(), "a1a2 b7b5 ");
    gs->setGameHistoryString("");
    EXPECT_EQ(gs->getGameHistoryString(), "");
}
TEST_F(GameStateTests, GameWinner) {
    // Check that the initial winner is nullptr
    EXPECT_EQ(gs->getWinner(), nullptr);

    // Make some moves to set up a checkmate

    std::string checkmateScenario = ".......k-\n.......\n........\n........\n........\n.......r-\n.......\n.r-.....K-\n";
    
    gs->getMutableBoard().initializeBoardFromString(checkmateScenario);
    gs->getCurrentPlayer()->setIsInCheck(true);
    gs->setGameStatus(GameStatus::CHECKMATE);
    
    // Check that the winner is player2 (black)
    EXPECT_EQ(gs->getWinner()->getColour(), Colour::BLACK);
}


TEST_F(GameStateTests, TurnStartTime) {
    // Test that the initial turn start time is very close to the current time
    auto now = std::chrono::steady_clock::now();
    auto diff = now - gs->getTurnStartTime();
    EXPECT_LT(diff, std::chrono::milliseconds(100));  // Assuming the difference should be less than 100 milliseconds

    // Set the turn start time to be 1 minute earlier and check the difference
    auto newStartTime = std::chrono::steady_clock::now() - std::chrono::minutes(1);
    gs->setTurnStartTime(newStartTime);
    now = std::chrono::steady_clock::now() - std::chrono::minutes(1);
    diff = now - gs->getTurnStartTime();
    EXPECT_LT(diff, std::chrono::milliseconds(100)); // Assuming the difference should be less than 100 milliseconds
}

TEST_F(GameStateTests, LastMoveTest) {
    // Test get and set LastMove
    Move lastMove = gs->getLastMove();
    Position pos = Position{0, 0};
    Position pos2 = Position{0, 0};
    EXPECT_EQ(lastMove.start, pos);
    EXPECT_EQ(lastMove.end, pos2);
    EXPECT_EQ(lastMove.pieceMoved, PieceType::NONE);
    EXPECT_EQ(lastMove.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(lastMove.playerColour, Colour::WHITE);

    Move newMove = Move(Position{1, 0}, Position{3, 0}, PieceType::PAWN, Colour::WHITE);
    gs->setLastMove(newMove);

    lastMove = gs->getLastMove();
    Position pos3 = Position{1, 0};
    Position pos4 = Position{3, 0};
    EXPECT_EQ(lastMove.start, pos3);
    EXPECT_EQ(lastMove.end, pos4);
    EXPECT_EQ(lastMove.pieceMoved, PieceType::PAWN);
    EXPECT_EQ(lastMove.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(lastMove.playerColour, Colour::WHITE);
}

TEST_F(GameStateTests, TurnNumberTest) {
    // Test get and increment TurnNumber
    EXPECT_EQ(gs->getTurnNumber(), 1);

    for (int i = 1; i < 10; i++) {
        gs->incrementTurnNumber();
    }
    EXPECT_EQ(gs->getTurnNumber(), 10);

    gs->switchTurns();
    EXPECT_EQ(gs->getTurnNumber(), 10);
    gs->switchTurns();
    EXPECT_EQ(gs->getTurnNumber(), 11);
}

TEST_F(GameStateTests, NoCaptureOrPawnMoveCounterTest) {
    // Test get and set NoCaptureOrPawnMoveCounter
    EXPECT_EQ(gs->getNoCaptureOrPawnMoveCounter(), 0);

    gs->setNoCaptureOrPawnMoveCounter(10);

    EXPECT_EQ(gs->getNoCaptureOrPawnMoveCounter(), 10);
}

TEST_F(GameStateTests, GameStatusTest) {
    // Test get and set GameStatus
    EXPECT_EQ(gs->getGameStatus(), GameStatus::ONGOING);

    gs->setGameStatus(GameStatus::CHECKMATE);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::CHECKMATE);
    EXPECT_EQ(gs->getCurrentPlayer()->getIsInCheck(), true);
    EXPECT_EQ(gs->getWinner()->getColour(), Colour::BLACK);

    gs->setGameStatus(GameStatus::STALEMATE);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::STALEMATE);

    gs->setGameStatus(GameStatus::CHECK);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::CHECK);
    EXPECT_EQ(gs->getCurrentPlayer()->getIsInCheck(), true);

    gs->setGameStatus(GameStatus::ONGOING);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::ONGOING);
    EXPECT_EQ(gs->getCurrentPlayer()->getIsInCheck(), false);

    gs->setGameStatus(GameStatus::PROMPTDRAW);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::PROMPTDRAW);

    gs->setGameStatus(GameStatus::DRAW);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::DRAW);

    gs->setGameStatus(GameStatus::RESIGN);
    EXPECT_EQ(gs->getGameStatus(), GameStatus::RESIGN);
    EXPECT_EQ(gs->getCurrentPlayer()->getIsResigning(), true);
    EXPECT_EQ(gs->getCurrentPlayer()->getIsInCheck(), true);
    EXPECT_EQ(gs->getWinner()->getColour(), Colour::BLACK);
}


TEST_F(GameStateTests, SwitchTurnsTest) {
    // Initially, Alice is the current player and Bob is the opponent
    EXPECT_EQ(gs->getCurrentPlayer()->getName(), "Alice");
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "Bob");

    // After switching turns, Bob should become the current player and Alice the opponent
    gs->switchTurns();

    EXPECT_EQ(gs->getCurrentPlayer()->getName(), "Bob");
    EXPECT_EQ(gs->getOpponentPlayer()->getName(), "Alice");
}

TEST_F(GameStateTests, ParsingMoveStrings) {
    std::string moveString = "a2a4";

    Move move = gs->parseMoveString(moveString);

    EXPECT_EQ(move.start, Position(6, 0));
    EXPECT_EQ(move.end, Position(4, 0));
    EXPECT_EQ(move.pieceMoved, PieceType::PAWN);
    EXPECT_EQ(move.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(move.playerColour, Colour::WHITE);

    moveString = "a2a4 ";

    // catch thrown exceptions 
    EXPECT_THROW(gs->parseMoveString(moveString), std::invalid_argument);

    moveString = "a2a4a";
    EXPECT_THROW(gs->parseMoveString(moveString), std::invalid_argument);

    moveString = "x2y4";
    EXPECT_THROW(gs->parseMoveString(moveString), std::invalid_argument);

    moveString = "a2a9";
    EXPECT_THROW(gs->parseMoveString(moveString), std::invalid_argument);

    moveString = "a2a";
    EXPECT_THROW(gs->parseMoveString(moveString), std::invalid_argument);

    moveString = "g1f3";
    move = gs->parseMoveString(moveString);
    EXPECT_EQ(move.start, Position(7, 6));
    EXPECT_EQ(move.end, Position(5, 5));
    EXPECT_EQ(move.pieceMoved, PieceType::KNIGHT);
    EXPECT_EQ(move.pieceCaptured, PieceType::NONE);
    EXPECT_EQ(move.playerColour, Colour::WHITE);
}