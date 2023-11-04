/**
 * @file test_GameController.cpp
 * @brief Unit tests for the GameController class.
 *
 * This file contains unit tests for the GameController class, including constructor initialization, startGame initialization, validateMove, makeMove, getPossibleMoves, isStalemate, promotePawn, isKingInCheck, tryMove, isKingInCheckmate, and updateGameStatus methods.
 *
 * @author Brad McGlynn
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @author Chris Hosang
 * @date September 28th, 2023
 */
#include <gtest/gtest.h>
#include <vector>
#include <algorithm> 

#include "GameController.h"
#include "GameState.h"
#include "Piece.h"
#include "Player.h"
#include "User.h"
#include "UserManager.h"

/**
 * @brief Test fixture for GameController tests.
 */
class GameControllerTests : public testing::Test {
protected:
    User* user1;
    User* user2;
    std::unique_ptr<Player> whitePlayer;
    std::unique_ptr<Player> blackPlayer;
    GameController* controller;

    // Initialize the game controller 
    void SetUp() override {
        user1 = new User("Alice");
        user2 = new User("Bob");
        whitePlayer = std::make_unique<Player>(*user1, Colour::WHITE, std::chrono::minutes(5));
        blackPlayer = std::make_unique<Player>(*user2, Colour::BLACK, std::chrono::minutes(5));
        controller = new GameController(std::move(whitePlayer), std::move(blackPlayer), ":memory:");
    }

    // Delete user & player instances 
    void TearDown() override {
        delete user1;
        delete user2;
        delete controller;
    }
};


TEST_F(GameControllerTests, ConstructorInitialization) {
    // Just ensuring the gameState is not a nullptr after construction. 
    EXPECT_NE(controller->getGameState(), nullptr);
}

TEST_F(GameControllerTests, StartGameInitialization) {
    User user3("newAlice");
    User user4("newBob");
    std::unique_ptr<Player> newWhitePlayer = std::make_unique<Player>(user3, Colour::WHITE, std::chrono::minutes(5));
    std::unique_ptr<Player> newBlackPlayer = std::make_unique<Player>(user4, Colour::BLACK, std::chrono::minutes(5));

    controller->setGameStatus(GameStatus::CHECKMATE);
    EXPECT_EQ(controller->getGameStatus(), GameStatus::CHECKMATE);

    controller->startGame(std::move(newWhitePlayer), std::move(newBlackPlayer)); // Resets game with new players 

    EXPECT_EQ(controller->getGameStatus(), GameStatus::ONGOING);
    EXPECT_EQ(controller->getGameState()->getCurrentPlayer()->getColour(), Colour::WHITE);
    EXPECT_EQ(controller->getGameState()->getOpponentPlayer()->getColour(), Colour::BLACK);
}

TEST_F(GameControllerTests, SaveLoadGame) {
    // Make a test board sequence 
    controller->getCurrentPlayer()->setTimeLeft(std::chrono::minutes(1));
    controller->getOpponentPlayer()->setTimeLeft(std::chrono::minutes(2));
    controller->setGameStatus(GameStatus::CHECK);

    // Update board state metadata 
    std::string whitePlayerName = controller->getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? controller->getCurrentPlayer()->getName() : controller->getOpponentPlayer()->getName();

    std::string blackPlayerName = controller->getCurrentPlayer()->getColour() == Colour::BLACK 
                        ? controller->getCurrentPlayer()->getName() : controller->getOpponentPlayer()->getName();

    std::string pgn = generatePGN(controller->getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, controller->getGameStatus(), controller->getBoardStatesMetadata().first);

    controller->updateBoardStatesMetadata(pgn, getCurrentDate());

    controller->setGameStatus(GameStatus::PROMPTDRAW);

    pgn = pgn = generatePGN(controller->getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, controller->getGameStatus(), controller->getBoardStatesMetadata().first);

    controller->updateBoardStatesMetadata(pgn, getCurrentDate());

    controller->setGameStatus(GameStatus::ONGOING);

    pgn = generatePGN(controller->getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, controller->getGameStatus(), controller->getBoardStatesMetadata().first);
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());

    EXPECT_TRUE(controller->saveGame());

    std::unique_ptr<Player> whitePlayer2 = std::make_unique<Player>(*user1, Colour::WHITE, std::chrono::minutes(5));
    std::unique_ptr<Player> blackPlayer2 = std::make_unique<Player>(*user2, Colour::BLACK, std::chrono::minutes(5));
    
    controller->startGame(std::move(whitePlayer2), std::move(blackPlayer2));
    int game = controller->getCurrentPlayer()->getUser().getGames()[0];

    EXPECT_TRUE(controller->loadGame(game));

    std::vector<BoardMetadata> loadedMetadata = controller->getBoardStatesMetadata().first;

    std::cout <<"2 \n " <<  loadedMetadata[0].PGNstring << std::endl;
    std::cout << "3 \n " << loadedMetadata[1].PGNstring << std::endl;
    std::cout << "4 \n " << loadedMetadata[2].PGNstring << std::endl;

    EXPECT_EQ(loadedMetadata[0].gameStatus, GameStatus::CHECK);
    EXPECT_EQ(loadedMetadata[1].gameStatus, GameStatus::PROMPTDRAW);
    EXPECT_EQ(loadedMetadata[2].gameStatus, GameStatus::ONGOING);

    EXPECT_EQ(controller->getGameState()->getCurrentPlayer()->getTimeLeft(), std::chrono::minutes(1));
    EXPECT_EQ(controller->getGameState()->getOpponentPlayer()->getTimeLeft(), std::chrono::minutes(2));
}


TEST_F(GameControllerTests, ValidateMove) {
    // Test for an obvious invalid move: moving a piece to its own location. 
    Position start(0, 0);
    Position end(0, 0);
    EXPECT_FALSE(controller->validateMove(start, end));

    // Assuming initial setup: Test for a pawn's basic move 
    Position pawnStart(1, 0);
    Position pawnEnd(2, 0);
    EXPECT_TRUE(controller->validateMove(pawnStart, pawnEnd));

    // Test for a move that's out of bounds 
    Position outOfBounds(9, 9);
    EXPECT_FALSE(controller->validateMove(start, outOfBounds));
}

TEST_F(GameControllerTests, MakeMove) {
    // Test for an invalid move 
    Position start(0, 0);
    Position end(0, 0);
    EXPECT_FALSE(controller->makeMove(start, end));

    // Assuming initial setup: Test for a pawn's basic move 
    Position pawnStart(6, 0);
    Position pawnEnd(5, 0);
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::WHITE); // Make sure current player is White before move is made 
    EXPECT_TRUE(controller->makeMove(pawnStart, pawnEnd));
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK); // Make sure current player is Black after move is made to ensure switchTurns() was called 

    // Test for a move that's out of bounds 
    Position outOfBounds(9, 9);
    EXPECT_FALSE(controller->makeMove(start, outOfBounds));

    // Test for a move that's not a pawn's basic move (horizontal movement) 
    Position notPawnStart(0, 0);
    Position notPawnEnd(0, 2);
    EXPECT_FALSE(controller->makeMove(notPawnStart, notPawnEnd));

    // Test to see that castling works 
    controller->getGameState()->getMutableBoard().initializeBoardFromString("r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\n........\n........\nP-P-P-P-P-P-P-P-\nR-...K-..R-\n");
    Position kingStart(7, 4);
    Position kingEnd(7, 6);
    Position rookStart(7, 7);
    Position rookEnd(7, 5);
    EXPECT_TRUE(controller->makeMove(kingStart, kingEnd));
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(kingEnd)->getPieceType(), PieceType::KING);
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(rookEnd)->getPieceType(), PieceType::ROOK);
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(rookEnd)->getColour(), Colour::WHITE);
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(kingEnd)->getColour(), Colour::WHITE);
    EXPECT_TRUE(controller->getGameState()->getMutableBoard().getPieceAtPosition(rookEnd)->getHasMoved());
    EXPECT_TRUE(controller->getGameState()->getMutableBoard().getPieceAtPosition(kingEnd)->getHasMoved());
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(kingStart), nullptr);
    EXPECT_EQ(controller->getGameState()->getMutableBoard().getPieceAtPosition(rookStart), nullptr);

}

TEST_F(GameControllerTests, GetPossibleMoves) {
    // Test for an empty square 
    Position emptySquare(3, 3);
    auto movesForEmptySquare = controller->getPossibleMoves(emptySquare);
    EXPECT_TRUE(movesForEmptySquare.empty());

    // Test for a white pawn at the beginning of the game 
    Position pawnPosition(1, 0);
    auto movesForPawn = controller->getPossibleMoves(pawnPosition);
    std::vector<Position> expectedMoves = {{2, 0}, {3, 0}}; // Assuming pawns can move two spaces on their first move 

    EXPECT_EQ(movesForPawn.size(), expectedMoves.size());
    for(const auto& move : expectedMoves) {
        EXPECT_NE(std::find(movesForPawn.begin(), movesForPawn.end(), move), movesForPawn.end());
    }
}

TEST_F(GameControllerTests, IsStalemate) {
    // Scenario: Test initial board setup. It's not a stalemate at the beginning. 
    EXPECT_FALSE(controller->isStalemate());

    controller->getGameState()->getMutableBoard().initializeBoardFromString(".......k-\n....p-.p-.\n....Q-.Q-.\n........\n........\n........\n........\n...K-....\n");
    controller->switchTurns(); // Current player is now black, meaning they are now in stalemate as thy can't move any pieces legally 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK);
    EXPECT_TRUE(controller->isStalemate());
    
}

TEST_F(GameControllerTests, PromotePawn) {
    // Clear board, then set it up with a test scenario to show promotion 
    controller->getGameState()->getMutableBoard().initializeBoardFromString(".......k-\nP-.......\n........\n........\n..p-p-....\n........\n........\n...K-....\n");
    /* Scenario: Pawn not at promotion row */
    Position notPromotionRow(1, 0);
    EXPECT_FALSE(controller->promotePawn(notPromotionRow, PieceType::QUEEN));

    // Scenario: Pawn at promotion row 
    Position promotionRow(0, 0);
    // Assume you moved a pawn to the 7th row beforehand: 
    controller->makeMove(notPromotionRow, promotionRow);
    EXPECT_TRUE(controller->getGameState()->getMutableBoard().getPieceAtPosition(promotionRow)->getPieceType() == PieceType::PAWN);
    EXPECT_TRUE(controller->getGameState()->getMutableBoard().getPieceAtPosition(promotionRow)->getColour() == Colour::WHITE);
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::WHITE);
    EXPECT_TRUE(controller->promotePawn(promotionRow, PieceType::QUEEN));
    
    // Verify the pawn is now a queen 
    auto piece = controller->getGameState()->getMutableBoard().getPieceAtPosition(promotionRow);
    EXPECT_EQ(piece->getPieceType(), PieceType::QUEEN);
    EXPECT_NE(piece->getPieceType(), PieceType::PAWN);
    EXPECT_EQ(piece->getColour(), Colour::WHITE);
}

TEST_F(GameControllerTests, IsKingInCheck) {
    // Scenario: Test initial board setup. King shouldn't be in check at the beginning. 
    EXPECT_FALSE(controller->isKingInCheck());

    controller->getGameState()->getMutableBoard().initializeBoardFromString("Q-......k-\n........\n........\n........\n..p-p-....\n........\n........\n...K-....\n");
    controller->switchTurns(); // Current player is now black, meaning their king is in check 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK);
    EXPECT_TRUE(controller->isKingInCheck());
    
}

TEST_F(GameControllerTests, TryMove) {
    controller->getGameState()->getMutableBoard().initializeBoardFromString("......p-k-\n....p-...\n....Q-Q-Q-.\n.....Q-..\n........\n........\n........\n...K-....\n");
    // Scenario: King trapped in corner with a single move available to a pawn to get it out of check 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::WHITE);
    controller->switchTurns(); // Current player is now black, meaning their king is in check 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK);
    EXPECT_TRUE(controller->isKingInCheck());
    Position start(0, 6);  // Pawn in front of king 
    Position end(1, 6);
    EXPECT_TRUE(controller->tryMove(start, end));
    controller->makeMove(start, end);
    EXPECT_FALSE(controller->isKingInCheck());

    // Scenario: Trying an invalid move, captuing an opponent queen while the king is still in check 
    // Current player is now white, meaning they can attack the king 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::WHITE);
    controller->makeMove(Position{2,6}, Position{1,7}); // Moving queen to put the king in check 
    // Current player is now black, meaning their king is in check 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK);
    EXPECT_TRUE(controller->isKingInCheck());
    Position newStart(1, 6);  // Pawn diagonally in front of king to left 
    Position newEnd(2, 5);
    EXPECT_FALSE(controller->tryMove(start, end));
}

TEST_F(GameControllerTests, IsKingInCheckmate) {
    EXPECT_FALSE(controller->isKingInCheckmate());
    EXPECT_FALSE(controller->isKingInCheck()); // Both checks should be false in an initial board state 

    controller->getGameState()->getMutableBoard().clearBoard();
    controller->getGameState()->getMutableBoard().initializeBoardFromString("......k-.\n....p-.p-.\n....Q-Q-Q-.\n.....Q-..\n........\n........\n........\n...K-....\n");
    // Scenario: King trapped in corner, white to move 
    controller->switchTurns(); // Current player is now black, meaning their king is in check 
    EXPECT_TRUE(controller->isKingInCheck()); // King is currently in check, now black must move 
    Position start(0, 6);  // king is in check, so only one move is available 
    Position end(0, 7);
    EXPECT_TRUE(controller->tryMove(start, end));
    controller->makeMove(start, end); // this makes white the current player 
    controller->switchTurns(); // swap back to black just to check that their king isn't in check 
    controller->switchTurns(); // swap back to white 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::WHITE);
    
    controller->makeMove(Position{2,6}, Position{1,7}); // moving queen to put the king in check 
    // current player is now black, meaning their king is in check 
    EXPECT_EQ(controller->getCurrentPlayer()->getColour(), Colour::BLACK);
    EXPECT_TRUE(controller->isKingInCheck());
    EXPECT_TRUE(controller->isKingInCheckmate());

}

TEST_F(GameControllerTests, UpdateGameStatus) {
    // Scenario: Initial board setup. Game should be ongoing. 
    EXPECT_EQ(controller->getGameStatus(), GameStatus::ONGOING);

    // Checkmate scenario 
    std::string expectedBoardState = "Q-.....k-.\n....p-.p-.\n....Q-Q-Q-.\n.....Q-..\n........\n........\n........\n...K-....\n";
    controller->getGameState()->getMutableBoard().initializeBoardFromString(expectedBoardState);
    controller->switchTurns(); // current player is now black, meaning their king is in check 
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameState()->getGameStatus(), GameStatus::CHECKMATE);
    

    // Stalemate scenario 
    controller->getGameState()->getMutableBoard().initializeBoardFromString(".......k-\n....p-.p-.\n....Q-.Q-.\n........\n........\n........\n........\n...K-....\n");
    // current player is still now black, meaning they are now in stalemate as thy can't move any pieces legally 
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::STALEMATE);
    

    // Check scenario 
    expectedBoardState = "Q-......k-\n........\n........\n........\n..p-p-....\n........\n........\n...K-....\n";
    controller->getGameState()->getMutableBoard().initializeBoardFromString(expectedBoardState);
    // current player is still black, meaning their king is in check 
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::CHECK);

    // Repetition scenarios (threefold, fivefold, Fifty-move, 35 move) 
    expectedBoardState = "r-n-b-q-k-b-n-r-\np-p-p-p-p-p-p-p-\n........\n........\n........\n........\nP-P-P-P-P-P-P-P-\nR-N-B-Q-K-B-N-R-\n";
    controller->getGameState()->getMutableBoard().initializeBoardFromString(expectedBoardState);
    
    // Update board state metadata 
    std::string whitePlayerName = controller->getCurrentPlayer()->getColour() == Colour::WHITE 
                        ? controller->getCurrentPlayer()->getName() : controller->getOpponentPlayer()->getName();

    std::string blackPlayerName = controller->getCurrentPlayer()->getColour() == Colour::BLACK 
                        ? controller->getCurrentPlayer()->getName() : controller->getOpponentPlayer()->getName();

    std::string pgn = generatePGN(controller->getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, controller->getGameStatus(), controller->getBoardStatesMetadata().first);
    std::cout << "5 \n " << pgn << std::endl;



    // threefold repetition is when the same board state occurs three times in a game 
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::PROMPTDRAW);

    // Fivefold repetition scenario 
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());
    controller->updateBoardStatesMetadata(pgn, getCurrentDate());
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::DRAW); // automatic draw after the same board repeats 5 times 

    pgn = generatePGN(controller->getCurrentPlayer()->getName(), whitePlayerName, blackPlayerName, controller->getGameStatus(), controller->getBoardStatesMetadata().first);

    std::cout << "6 \n " << pgn << std::endl;
    
    // Fifty-move scenario 
    controller->getMutableBoard().initializeBoard();
    controller->makeMove(Position{6, 4}, Position{4, 4}); // White's move e2e4
    controller->setNoCaptureOrPawnMoveCounter(50);
    controller->setLastMove(Move(Position{7, 1}, Position{5, 0}, PieceType::KNIGHT, Colour::WHITE)); // set last move to random move that isn't a pawn and wasn't a capture 
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::PROMPTDRAW); // prompt players to draw after 50 moves of no captures or no pawn movement 

    // 75 move scenario 
    controller->setNoCaptureOrPawnMoveCounter(75);
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::DRAW); // automatic draw after 75 moves of no captures or no pawn movement

    // Dead position scenario 
    expectedBoardState = ".......k-\n......n-\n........\n........\n........\n........\n........\n.......K-\n";
    controller->getGameState()->getMutableBoard().initializeBoardFromString(expectedBoardState);
    controller->updateGameStatus();
    EXPECT_EQ(controller->getGameStatus(), GameStatus::DRAW);

}

TEST_F(GameControllerTests, StockfishOutputDifferentInitialState) {

    // Simulate a game with different initial moves 
    controller->makeMove(Position{6, 4}, Position{4, 4}); // White's move e2e4 
    controller->makeMove(Position{1, 3}, Position{3, 3}); // Black's move d7d5 
    controller->makeMove(Position{7, 6}, Position{5, 5}); // White's move g1f3 
    controller->makeMove(Position{3, 3}, Position{4, 3}); // Black's move d5d4 
    controller->makeMove(Position{6, 0}, Position{5, 0}); // White's move a2a3 

    // Query Stockfish for its moves based on the current game state 
    std::vector<Move> stockfishMoves = controller->generateStockfishMoves();

    // Validate Stockfish's output 
    EXPECT_FALSE(stockfishMoves.empty());
    EXPECT_TRUE(stockfishMoves[0].start == Position(4, 3) && stockfishMoves[0].end == Position(5, 3));


    // Test with different stockfish skill levels 
    controller->setStockfishDifficulty(Difficulty::MEDIUM);
    EXPECT_TRUE(controller->getStockfishDifficulty() == Difficulty::MEDIUM);
    stockfishMoves = controller->generateStockfishMoves();

    EXPECT_TRUE(stockfishMoves[0].start == Position(1, 2) && stockfishMoves[0].end == Position(2, 2));

    controller->setStockfishDifficulty(Difficulty::EASY);
    EXPECT_TRUE(controller->getStockfishDifficulty() == Difficulty::EASY);
    stockfishMoves = controller->generateStockfishMoves();

    EXPECT_TRUE(stockfishMoves[0].start == Position(1, 2) && stockfishMoves[0].end == Position(2, 2)); // same as medium difficulty, more pronounced differences later in games
}

TEST_F(GameControllerTests, StockfishOutputEmptyInitialState) {
    // Query Stockfish for its moves with an empty initial state
    std::vector<Move> stockfishMoves = controller->generateStockfishMoves();

    // stockfish should supply a move to start the game
    EXPECT_FALSE(stockfishMoves.empty());
    // verify stockfish output based on the skill level / difficulty of the AI, default = 20 (hardest)
    EXPECT_TRUE(stockfishMoves[0].start == Position(6, 0) && stockfishMoves[0].end == Position(5, 0));
    EXPECT_TRUE(controller->getStockfishDifficulty() == Difficulty::HARD);

    controller->setStockfishDifficulty(Difficulty::MEDIUM);
    EXPECT_TRUE(controller->getStockfishDifficulty() == Difficulty::MEDIUM);
    stockfishMoves = controller->generateStockfishMoves();

    EXPECT_TRUE(stockfishMoves[0].start == Position(6, 3) && stockfishMoves[0].end == Position(5, 3));


    controller->setStockfishDifficulty(Difficulty::EASY);
    EXPECT_TRUE(controller->getStockfishDifficulty() == Difficulty::EASY);
    stockfishMoves = controller->generateStockfishMoves();
    EXPECT_TRUE(stockfishMoves[0].start == Position(6, 3) && stockfishMoves[0].end == Position(5, 3)); // same as medium difficulty, more pronounced differences later in games 
}

