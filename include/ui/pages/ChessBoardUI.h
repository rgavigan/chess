/**
 * @file ChessBoardUI.h
 * @author Nicholas Corcoran (ncorcor@uwo.ca)
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @author Satvir Singh Uppal (suppal46@uwo.ca)
 * @brief This class Displays the Board.
 * This class is responsible for creating the main container that holds all other
 * sub-containers.
 * @date 2023-10-14
 */

#ifndef CHESS_BOARD_UI_H
#define CHESS_BOARD_UI_H

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WObject.h>
#include <Wt/WLink.h>
#include <Wt/WTimer.h>
#include <Wt/WBreak.h>
#include <Wt/WPopupWidget.h>
#include <Wt/WTable.h>
#include <Wt/WTableRow.h>
#include <sstream>
#include <Wt/WDialog.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>


#include "GameController.h"
#include "Piece.h"
#include "DataUtil.h"
#include "User.h"
#include "Player.h"
#include "UserStatistics.h"
#include "WStackedWidget.h"

class ChessBoardUI : public Wt::WObject {
public:
  ChessBoardUI(WStackedWidget* container, GameController *gameController);
  ~ChessBoardUI();

  void initializeComponents(User *user1, User *user2, UserStatistics* userOneStatistics, UserStatistics* userTwoStatistics);
  void startGame(); /** Method to start a new game */
  void startGameFromID(int id); /** Start a game from a given id */
  void startFromTest(std::string boardString, Colour turn); /** Start a game from the test board */
  void resetDefaultSquares(); /** Reset to the default squares */
  void gameHistory(); /** Displays the game History */

protected:
  const std::string *style_ = new std::string("default"); /** Style of the chess board */
  const std::string *checkedStyle_ = new std::string("checked-square"); /** Style of the checked square */
  const std::string *selectedStyle_ = new std::string("selected-square"); /** Style of the selected square */
  const std::string *possibleMoveStyle_ = new std::string("possible-move-square"); /** Style of the possible move square */

  Wt::WContainerWidget *container_; /** Container for the chess board page */
  Wt::WGridLayout *boardLayout_; /** Grid layout of the chess board */
  WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */
  Wt::WPushButton* selectedPiece_ = nullptr; /** First selected chess piece when making a move */

  Wt::WContainerWidget *pageContainer_;
  Wt::WGridLayout *pageLayout_;
  Wt::WPushButton *homepageButton_; /** Button to navigate back to homepage, stopping game */

  GameController *gameController_; /** Game controller instance */
  std::unordered_map<std::string, Wt::WPushButton*> chessBoard_;
  std::vector<Position> possibleMoves_; /** Possible moves for the selected piece */

  void initializeBoard(); /** Initialize the chess board */

  void updateBoard(); /** Update the board with the current board state */
  std::vector<Position> updatePossibleMoves(); /** Update the possible moves for the selected piece */
  void handleButtonPress(Wt::WPushButton* btn);

  void updateButtonStyleClass(Wt::WPushButton* btn, const std::string* styleClass); /** Helper to update button style class */
  void updateStylesForPositions(std::vector<Position>& positions, const std::string* styleClass); /** Helper to update the style of a position vector to styleClass */
  void updateTable(); /** Updates The Game table */

private:
  User *user1_; /** User 1 instance */
  User *user2_; /** User 2 instance */
  std::vector<Position> checkPieces_; /** Pieces that are involved in checking the current player */
  UserStatistics *userOneStatistics_; /** User one statistics instance */
  UserStatistics *userTwoStatistics_; /** User two statistics instance */
  Wt::WContainerWidget *gameInfo_; /** Container for the game info */
  Wt::WContainerWidget *gameHistory_; /** Container for the game history  */
  Wt::WTable* gameTable_; /** Table for The game History */

  Wt::WTimer *timer_; /** Timer to decrement current player time */

  Wt::WPushButton *blackResignButton_; /** Button for black player to resign */
  Wt::WPushButton *whiteResignButton_; /** Button for white player to resign */
  Wt::WPushButton *whiteDrawButton_; /** Button for white to prompt draw */
  Wt::WPushButton *blackDrawButton_; /* Button for black to prompt draw */

  Wt::WDialog *promptPromotePawnDialog_;  /** dialog for prompt draw */
  Wt::WDialog *promptSavedGameDialog_;  /** dialog for saved game feedback */
  Wt::WDialog *promptDrawDialog_;  /** dialog for prompt draw */
  Wt::WDialog *drawDialog_;  /** dialog for the draw */
  Wt::WDialog *winDialog_;  /** dialog for the win */
  Wt::WPushButton *saveGameButton_; /** Button to save the current game */

  Wt::WContainerWidget *winnerContainer_; /** Container for the winner */

  void makeMove(Wt::WPushButton* selectedPiece, Wt::WPushButton* destination); /** Attempt to make a move on the board */

  void updateForGameStatus(); /** Updates the UI based on the current game status */
  void decrementCurrentPlayerTime(); /** Decrement current player time by 1 second */

  void saveGame(); /** Save the current game given a button press*/

  void updateGameInfoPanel(std::string* currentColour, std::string* gameStatusString); /** Helper to update the game info panel for a changed game status */

  void handlePawnPromotion(Position end); /** Handles pawn promotion */
  void handlePromptDrawDialog(); /** Handles prompt draw dialog */
  void handleDrawDialog(); /** Handles draw dialog */
  void handleWinDialog(); /** Handles win dialog */

  void handleBlackPlayerResignation(); /** Handle black player resignation */
  void handleWhitePlayerResignation(); /** Handle white player resignation */

  void backToHomepage(); /** Handle navigation back to homepage */
  
};

#endif // CHESS_BOARD_UI_H