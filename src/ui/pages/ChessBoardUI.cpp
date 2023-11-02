/**
 * @file ChessBoardUI.cpp
 * @author Nicholas Corcoran (ncorcor@uwo.ca)
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This class contains the chess board page
 * @date 2023-10-14
 */

#include "ChessBoardUI.h"

/**
 * @brief Constructor for the Chess Board UI page
 * @param container Parent container for the chessboard UI.
 * @param gameController Pointer to the game controller.
 * @param user1 Pointer to the first user.
 * @param user2 Pointer to the second user.
 * @param userOneStatistics Pointer to the first user's statistics.
 * @param userTwoStatistics Pointer to the second user's statistics.
 * @param timer Timer to decrement current player time.
 */
ChessBoardUI::ChessBoardUI(Wt::WStackedWidget* container, GameController *gameController, User *user1, User *user2, UserStatistics* userOneStatistics, UserStatistics* userTwoStatistics)
{
  // Setting class variables from parameters 
  parentContainer_ = container;
  gameController_ = gameController;
  user1_ = user1;
  user2_ = user2;
  userOneStatistics_ = userOneStatistics;
  userTwoStatistics_ = userTwoStatistics;

  // Container for the page itself to hold the board and other necessary components 
  pageContainer_ = parentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
  pageContainer_->setStyleClass("centered-game-container");
  pageLayout_ = pageContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

  // Container to hold the chess board 
  container_ = pageLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 2 ,0, 1, 1);
  container_->setStyleClass("centered-board-container");

  // Layout for the chess board 
  boardLayout_ = container_->setLayout(std::make_unique<Wt::WGridLayout>());

  // User statistics containers within layout 
  pageLayout_->addWidget(userOneStatistics->getGameStatisticsContainer(), 3, 0);
  pageLayout_->addWidget(userTwoStatistics->getGameStatisticsContainer(), 1, 0);
  
  // Container that states current turn and other info 
  gameInfo_ = pageLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 2, 1);
  gameInfo_->setStyleClass("sidebar-container");
  gameInfo_->setContentAlignment(Wt::AlignmentFlag::Center);

  // Resign button for black player 
  blackResignButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 1, 1);
  blackResignButton_->setStyleClass("button");
  blackResignButton_->setObjectName("black");
  blackResignButton_->setText("Resign");
  blackResignButton_->clicked().connect([this] {
    handleBlackPlayerResignation();
  });

  // Resign button for white player 
  whiteResignButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 3, 1);
  whiteResignButton_->setStyleClass("button");
  whiteResignButton_->setObjectName("white");
  whiteResignButton_->setText("Resign");
  whiteResignButton_->clicked().connect([this] {
    handleWhitePlayerResignation();
  });

  // Homepage button to navigate back to homepage and stop the match
  homepageButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 0, 0);
  homepageButton_->setStyleClass("red-button");
  homepageButton_->setText("Back to Homepage");
  homepageButton_->clicked().connect([this] {
    backToHomepage();
  });

  // Save game button
  saveGameButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 0, 1);
  saveGameButton_->setStyleClass("red-button");
  saveGameButton_->setText("Save Game");

  // Timer to update counters
  timer_ = new Wt::WTimer();
  timer_->setInterval(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1)));
  timer_->timeout().connect(this, &ChessBoardUI::decrementCurrentPlayerTime);

  // Initialize the chess board 
  initializeBoard();
}

/**
 * @brief Destructor for the ChessBoardUI class.
 */
ChessBoardUI::~ChessBoardUI() {}

/**
 * @brief Updates the board based on the current state of the game
 */
void ChessBoardUI::updateBoard(){
  // Loop through the board's coordinates and update image based on the piece present 
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Position position(i, j);
      Piece* piece = gameController_->getPieceAtPosition(position);

      if (piece == nullptr || piece->getPieceType() == PieceType::NONE) {
        // No piece at the position - set the icon to empty 
        std::string coordinate = "(" + std::to_string(i) + "," + std::to_string(j) + ")";
        chessBoard_[coordinate]->setIcon("");
        continue;
      }

      // Get the piece type and make it a string with first letter capitalized 
      PieceType pieceType = piece->getPieceType();
      std::string pieceTypeString = 
        pieceType == PieceType::PAWN ? "Pawn" :
        pieceType == PieceType::ROOK ? "Rook" :
        pieceType == PieceType::KNIGHT ? "Knight" :
        pieceType == PieceType::BISHOP ? "Bishop" :
        pieceType == PieceType::QUEEN ? "Queen" :
        pieceType == PieceType::KING ? "King" : "";
      
      // Get the colour of the square 
      Colour pieceColour = piece->getColour();
      std::string pieceColourString = pieceColour == Colour::WHITE ? "white" : "black";

      // Set the icon based on the piece type and colour 
      std::string coordinate = "(" + std::to_string(i) + "," + std::to_string(j) + ")";
      chessBoard_[coordinate]->setIcon(Wt::WLink("resources/images/" + pieceColourString + "-" + pieceTypeString + ".png"));
    }
  }
}

/**
 * @brief Attempt to make a move on the chess board
 * If the move is valid, makes the move and updates board
 * If the move is invalid, does nothing
 */
void ChessBoardUI::makeMove(Wt::WPushButton* selectedPiece, Wt::WPushButton* destination) {
  Position start(selectedPiece->objectName()[1] - '0', selectedPiece->objectName()[3] - '0');
  Position end(destination->objectName()[1] - '0', destination->objectName()[3] - '0');

  // Make move and update accordingly 
  if (gameController_->makeMove(start, end)) {
    updateBoard();
    updateForGameStatus();
  }
}

/**
 * @brief Updates the possible moves for the selected piece on the board
 */
std::vector<Position> ChessBoardUI::updatePossibleMoves() {
  if (selectedPiece_ == nullptr) {
    return {};
  }
  // Get the current player's colour 
  Colour currentPlayerColour = gameController_->getCurrentPlayer()->getColour();

  // Get the selected piece's position 
  Position selectedPiecePosition(selectedPiece_->objectName()[1] - '0', selectedPiece_->objectName()[3] - '0');

  // Get the possible moves for the selected piece 
  std::vector<Position> possibleMoves = gameController_->getPossibleMoves(selectedPiecePosition);

  // Loop through the possible moves and update the style class 
  updateStylesForPositions(possibleMoves, possibleMoveStyle_);
  return possibleMoves;
}

void ChessBoardUI::updateButtonStyleClass(Wt::WPushButton* btn, const std::string* styleClass) {
  if (*styleClass == *style_) {
    btn->setStyleClass(((btn->objectName()[1] - '0') + (btn->objectName()[3] - '0')) % 2 == 0 ? "white-square" : "black-square");
    return;
  }
  btn->setStyleClass(*styleClass);
}

/**
 * @brief Handles the button press logic.
 *
 * Determines the action based on whether it's the first or second button clicked during a move.
 * @param btn The button that was pressed.
 */
void ChessBoardUI::handleButtonPress(Wt::WPushButton* btn) {
  // Get the current player colour 
  Colour currentPlayerColour = gameController_->getCurrentPlayer()->getColour();

  // No selected piece 
  if (selectedPiece_ == nullptr) {
      std::string iconUrl = btn->icon().url();

      // Check if icon of the button is empty or the same colour 
      if ((iconUrl == "") || (iconUrl.find("white") != std::string::npos && currentPlayerColour == Colour::BLACK) || 
      (iconUrl.find("black") != std::string::npos && currentPlayerColour == Colour::WHITE)) {
        return;
      }

      // Go through possibleMoves_ and reset style 
      updateStylesForPositions(possibleMoves_, style_);

      selectedPiece_ = btn;
      updateButtonStyleClass(selectedPiece_, selectedStyle_);
  } 
  // A piece has already been selected - attempt a move if possible 
  else {
    std::string startColour = selectedPiece_->icon().url();
    std::string endColour = btn->icon().url();

    bool areBothBlack = 
      (startColour.find("black") != std::string::npos) && 
      (endColour.find("black") != std::string::npos);
    bool areBothWhite = 
      (startColour.find("white") != std::string::npos) && 
      (endColour.find("white") != std::string::npos);

    // Reset the styles for the possible moves 
    updateStylesForPositions(possibleMoves_, style_);

    // Selected piece is the same colour as the newly pressed piece 
    if (areBothBlack || areBothWhite) {
      updateButtonStyleClass(selectedPiece_, style_);
      selectedPiece_ = btn;
      updateButtonStyleClass(selectedPiece_, selectedStyle_);
    }
    // Handle the attempted move 
    else {
      makeMove(selectedPiece_, btn);
      updateButtonStyleClass(selectedPiece_, style_);
      selectedPiece_ = nullptr;
      possibleMoves_.clear();
    }
  }
  // Highlight possible moves for the selected piece if applicable 
  possibleMoves_ = updatePossibleMoves();
}

/**
 * @brief Creates the initial chess board by adding buttons with the desired background colour
 */
void ChessBoardUI::initializeBoard() {
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 8; j++){
      // Coordinate string and button for the square 
      std::string coordinate = "(" + std::to_string(i) + "," + std::to_string(j) + ")";
      std::unique_ptr<Wt::WPushButton> square = std::make_unique<Wt::WPushButton>();

      // Set name of the square to the coordinates, set the style class, and add click handler 
      square->setObjectName(coordinate);
      updateButtonStyleClass(square.get(), style_);

      // Add button to the board 
      chessBoard_[coordinate] = square.get();
      boardLayout_->addWidget(std::move(square), i, j);
    }
  }
  // Initialize click handler for chess squares 
  for (auto& pair : chessBoard_) {
    pair.second->clicked().connect([this, &pair] {
      handleButtonPress(pair.second);
    });
  }
  updateBoard();
  updateForGameStatus();
}

/**
 * @brief Helper to update the style of positions in a vector to styleClass
 * 
 * Used to convert squares to either default white/black squares or other styles
 * such as check and possible moves
 * @param positions the vector of positions to reset
 */
void ChessBoardUI::updateStylesForPositions(std::vector<Position>& positions, const std::string* styleClass) {
  // Loop through positions and update style class 
  for (Position position : positions) {
    std::string coordinate = "(" + std::to_string(position.row) + "," + std::to_string(position.col) + ")";
    updateButtonStyleClass(chessBoard_[coordinate], styleClass);
  }
}

/**
 * @brief Helper to update the game info panel based on the game status
 * 
 * Sets the Current Player and Match Status text
 * @param currentColour the current player's colour
 * @param gameStatusString the game status as a string
 */
void ChessBoardUI::updateGameInfoPanel(std::string* currentColour, std::string* gameStatusString) {
  Wt::WText* statusText;
  Wt::WText* winnerText;
  Wt::WText* playerText;
  //  If the game was won by a player, display winning information
  if (*gameStatusString == "Checkmate" || *gameStatusString == "Resign") {
    Player* winner = gameController_->getWinner();
    std::string *winnerColour = winner->getColour() == Colour::WHITE ? new std::string("White") : new std::string("Black");
    gameInfo_->clear();
    statusText = gameInfo_->addWidget(std::make_unique<Wt::WText>("Match Over: " + *gameStatusString));
    gameInfo_->addWidget(std::make_unique<Wt::WBreak>());
    winnerText = gameInfo_->addWidget(std::make_unique<Wt::WText>("Winner: " + *winnerColour));
    statusText->setStyleClass("title");
    winnerText->setStyleClass("title");
  }
  // If the game was a draw, display draw information
  else if (*gameStatusString == "Draw" || *gameStatusString == "Stalemate") {
    gameInfo_->clear();
    statusText = gameInfo_->addWidget(std::make_unique<Wt::WText>("Match Over: " + *gameStatusString));
    statusText->setStyleClass("title");
  }
  else {
    gameInfo_->clear();
    playerText = gameInfo_->addWidget(std::make_unique<Wt::WText>("Current Player: " + *currentColour));
    gameInfo_->addWidget(std::make_unique<Wt::WBreak>());
    statusText = gameInfo_->addWidget(std::make_unique<Wt::WText>("Match Status: " + *gameStatusString));
    statusText->setStyleClass("title");
    playerText->setStyleClass("title");
  }
}

/**
 * @brief Updates the UI based on the current game status
 * 
 * Called after every move to update the game information as necessary.
 * Handles: Ended game, Check, Checkmate, Stalemate, Draw, Resignation, and Ongoing
 */
void ChessBoardUI::updateForGameStatus() {
  GameStatus gameStatus = gameController_->getGameStatus();
  Player* currentPlayer = gameController_->getCurrentPlayer();

  // Map the current player's colour to a string 
  std::string *currentColour = currentPlayer->getColour() == Colour::WHITE ? new std::string("White") : new std::string("Black");

  // Map the game status to a string 
  std::string *gameStatusString = 
    gameStatus == GameStatus::CHECK ? new std::string("Check") :
    gameStatus == GameStatus::CHECKMATE ? new std::string("Checkmate") :
    gameStatus == GameStatus::STALEMATE ? new std::string("Stalemate") :
    gameStatus == GameStatus::DRAW ? new std::string("Draw") :
    gameStatus == GameStatus::RESIGN ? new std::string("Resign") :
    gameStatus == GameStatus::ONGOING ? new std::string("Ongoing") : new std::string("");

  // Update the game info panel for the colour and status 
  updateGameInfoPanel(currentColour, gameStatusString);

  // Remove check class from previous check pieces 
  updateStylesForPositions(checkPieces_, style_);

  if (gameStatus == GameStatus::CHECK) {
    // Update check pieces and add check class to them 
    checkPieces_ = gameController_->getCheckPieces();
    updateStylesForPositions(checkPieces_, checkedStyle_);
  }
  else if (gameStatus == GameStatus::ONGOING) {
    return;
  }
  else {
    gameController_->endGame();
    timer_->stop();
    updateGameInfoPanel(currentColour, gameStatusString);

    // Disable the chess board buttons 
    for (auto& pair : chessBoard_) {
      pair.second->disable();
    }
    userOneStatistics_->updateStatistics(nullptr);
    userTwoStatistics_->updateStatistics(nullptr);
  }
}

/**
 * @brief Decrements the current player's time by 1 second
 * 
 * Called every second by the timer to update the current player's time
 */
void ChessBoardUI::decrementCurrentPlayerTime() {
  // Update the current player's time left 
  gameController_->decrementPlayerTime(std::chrono::duration<double>(1));

  // Get the current player 
  Player* currentPlayer = gameController_->getCurrentPlayer();
  
  // Update the user statistics 
  Player* player1 = currentPlayer->getColour() == Colour::WHITE ? currentPlayer : gameController_->getOpponentPlayer();
  Player* player2 = currentPlayer->getColour() == Colour::BLACK ? currentPlayer : gameController_->getOpponentPlayer();
  userOneStatistics_->updateStatistics(player1);
  userTwoStatistics_->updateStatistics(player2);

  // If player's time left <= 0 seconds, end the game 
  if (gameController_->getRemainingTime() <= std::chrono::duration<double>(0)) {
    gameController_->playerResigns(currentPlayer);
    updateForGameStatus();
  }
}

/**
 * @brief Starts a new game of chess
 */
void ChessBoardUI::startGame() {
  // Start the timer 
  timer_->start();

  // Create players
  std::unique_ptr<Player> player1 = std::make_unique<Player>(*user1_, Colour::WHITE, user1_->getTimer());
  std::unique_ptr<Player> player2 = std::make_unique<Player>(*user2_, Colour::BLACK, user2_->getTimer());

  // Start the match for the game controller
  gameController_->startGame(std::move(player1), std::move(player2));

  // Update user statistics
  userOneStatistics_->updateStatistics(gameController_->getCurrentPlayer());
  userTwoStatistics_->updateStatistics(gameController_->getOpponentPlayer());

  // Update the board
  updateBoard();

  // Update the game info
  updateForGameStatus();

  // Enable the chess board buttons 
    for (auto& pair : chessBoard_) {
      pair.second->enable();
    }
}

/**
 * @brief Handle black player resigning when it is their turn
 */
void ChessBoardUI::handleBlackPlayerResignation() {
  if (gameController_->getGameStatus() == GameStatus::ONGOING) {
    // Resign if black player is the current player
    if (gameController_->getCurrentPlayer()->getColour() == Colour::BLACK) {
      gameController_->playerResigns(gameController_->getCurrentPlayer());
      updateForGameStatus();
    }
  }
}

/**
 * @brief Handle white player resigning when it is their turn
 */
void ChessBoardUI::handleWhitePlayerResignation() {
  if (gameController_->getGameStatus() == GameStatus::ONGOING) {
    // Resign if white player is the current player
    if (gameController_->getCurrentPlayer()->getColour() == Colour::WHITE) {
      gameController_->playerResigns(gameController_->getCurrentPlayer());
      updateForGameStatus();
    }
  }
}

/**
 * @brief Handle navigation back to homepage, stopping the match
 */
void ChessBoardUI::backToHomepage() {
  // Stop the timer and reset the game
  timer_->stop();
  gameController_->resetGameState();
  updateBoard();

  // Navigate to homepage
  parentContainer_->setCurrentIndex(5);
}
