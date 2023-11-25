/**
 * @file TestBoardUI.cpp
 * @author Chris Hosang
 * @brief This is the UI class for the test board page.
 * @date 2023-11-14
 */

#include "TestBoardUI.h"

/**
 * @brief Construct a new TestBoardUI page.
 * @param container Parent container reference.
 * @param gameController Game controller reference.
 * @param homepage Parent homepage reference.
*/
TestBoardUI::TestBoardUI(WStackedWidget *container, GameController *gameController) : ChessBoardUI(container, gameController) {
    // Homepage button to navigate back to homepage and stop the match
    homepageButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 0, 0);
    homepageButton_->setStyleClass("red-button");
    homepageButton_->setText("Back to Homepage");
    homepageButton_->clicked().connect([=] {
        // Reset board
        startTest(); 
        parentContainer_->setCurrentIndex(5, true);
    });

    // Buttons to convert to a normal chess game
    playAsWhite_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>("Play as White"), 0, 1);
    playAsWhite_->setStyleClass("button");
    playAsWhite_->clicked().connect([=] {
        handlePlayButton(Colour::WHITE);
    });

    playAsBlack_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>("Play as Black"), 0, 2);
    playAsBlack_->setStyleClass("button");
    playAsBlack_->clicked().connect([=] {
        handlePlayButton(Colour::BLACK);
    });

    // Initialize test board environment
    startTest();
}

/**
 * @brief Behaviour for when the Play As White or Play As Black buttons are clicked.
 * @param colour Colour of who to initialize play as.
*/
void TestBoardUI::handlePlayButton(Colour colour) {
    if(validateGameState(colour)) {
        homepage_->testToPlay(gameController_->getMutableBoard().getBoardStateAsString(), colour);
        parentContainer_->setCurrentIndex(2, true);
    } else {
        auto dialog = container_->addChild(std::make_unique<Wt::WDialog>("Invalid Board State"));
        dialog->contents()->addWidget(std::make_unique<Wt::WText>("The current board state is invalid, possibly because the other player is in check on your turn."));
        dialog->footer()->addWidget(std::make_unique<Wt::WPushButton>("OK"))
            ->clicked().connect([=] {
                dialog->accept();
        });

        dialog->show();
    }
}

void TestBoardUI::setHomepage(Homepage* homepage) {
    homepage_ = homepage;
}

/**
 * @brief Destructs the TestBoardUI page.
*/
TestBoardUI::~TestBoardUI() {}

/**
 * @brief Start a new test mode session.
*/
void TestBoardUI::startTest() {
    // Initialize test chess board (only kings)
    gameController_->getMutableBoard().initializeBoard(true);
    initializeBoard();
    
    // Piece maker buttons
    makerContainer_ = pageLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 2, 1);
    makerLayout_ = makerContainer_->setLayout(std::make_unique<Wt::WGridLayout>());
    initializePieceMaker();

    // Delete piece button
    deleteButton_ = pageLayout_->addWidget(std::make_unique<Wt::WPushButton>("Delete Selected Piece"), 3, 1);
    deleteButton_->setStyleClass("red-button");
    deleteButton_->clicked().connect([=] {
        // Delete the selected piece, if there is one
        if(selectedPiece_ != nullptr) {
            // Cannot delete the kings
            if(selectedPiece_->icon().url().find("King") == std::string::npos) {
                Position pos(selectedPiece_->objectName()[1] - '0', selectedPiece_->objectName()[3] - '0');
                gameController_->getMutableBoard().placePiece(pos, nullptr);
                updateBoard();
            }

            // Reset selection styling
            updateButtonStyleClass(selectedPiece_, style_);
            selectedPiece_ = nullptr;
            updateStylesForPositions(possibleMoves_, style_);
        }
    });
}

/**
 * @brief Initializes the piece maker buttons.
*/
void TestBoardUI::initializePieceMaker() {
    const std::vector<std::string> colours {"white", "black"}; 
    const std::vector<std::string> pieces {"Pawn", "Rook", "Bishop", "Knight", "Queen"}; 

    // Create all buttons
    for(int i = 0; i < colours.size(); i++) {
        std::string c = colours[i];
        for(int j = 0; j < pieces.size(); j++) {
            std::string p = pieces[j];

            std::string name = c + "-" + p;
            std::unique_ptr<Wt::WPushButton> btn = std::make_unique<Wt::WPushButton>();

            btn->setIcon(Wt::WLink("resources/images/" + name + ".png"));
            pieceMaker_[name] = btn.get();

            btn->setObjectName(name);
            makerLayout_->addWidget(std::move(btn), j, i);
        }
    }

    // Give each button click behaviour
    for(auto& pair : pieceMaker_) {
        pair.second->clicked().connect([this, &pair] {
            handleMakerClicked(pair.first, pair.second);
        });
    }
}

/**
 * @brief Initializes the chess board.
*/
void TestBoardUI::initializeBoard() {
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
}

/**
 * @brief Behaviour for when a piece maker button is clicked.
 * @param name Name of the piece being created, in the format "colour-Piece" (e.g. white-Pawn).
 * @param btn Reference to the button being clicked.
*/
void TestBoardUI::handleMakerClicked(std::string name, Wt::WPushButton* btn) {
    updateMakerStyle();
    btn->decorationStyle().setBorder(Wt::WBorder(Wt::BorderStyle::Dashed, Wt::BorderWidth::Medium));
    pieceToMake_ = name;
}

/**
 * @brief Resets the button styling of the piece maker buttons.
*/
void TestBoardUI::updateMakerStyle() {
    for(auto& pair : pieceMaker_) {
        pair.second->decorationStyle().setBorder(Wt::WBorder(Wt::BorderStyle::Solid, Wt::BorderWidth::Thin));
    }
}

/**
 * @brief Handles user input on the chess board.
 * @param btn Space the user clicked on.
*/
void TestBoardUI::handleButtonPress(Wt::WPushButton* btn) {
    std::string iconUrl = btn->icon().url();
    
    // If a piece maker button is selected
    if(pieceToMake_ != "") {
        // Cannot overwrite the kings
        if(iconUrl.find("King") == std::string::npos) {
            Colour newColour = pieceToMake_.find("white") != std::string::npos ? Colour::WHITE : Colour::BLACK;
            Position newPosition(btn->objectName()[1] - '0', btn->objectName()[3] - '0');
            PieceType newPieceType = pieceToMake_.find("Pawn") != std::string::npos ? PieceType::PAWN :
                                     pieceToMake_.find("Rook") != std::string::npos ? PieceType::ROOK :
                                     pieceToMake_.find("Bishop") != std::string::npos ? PieceType::BISHOP :
                                     pieceToMake_.find("Knight") != std::string::npos ? PieceType::KNIGHT : PieceType::QUEEN;


            std::unique_ptr<Piece> newPiece = PieceFactory::createPiece(newColour, newPosition, newPieceType);

            gameController_->getMutableBoard().placePiece(newPosition, std::move(newPiece));
            updateBoard();
        }
        
        updateMakerStyle();
        pieceToMake_ = "";
    }
    
    // No selected piece
    else if (selectedPiece_ == nullptr) {
        // Check if icon of the button is empty
        if (iconUrl == "") {
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
 * @brief Moves a piece from one location on the board to another.
 * @param selectedPiece Moving piece.
 * @param destination Space to move the piece to.
*/
void TestBoardUI::makeMove(Wt::WPushButton* selectedPiece, Wt::WPushButton* destination) {
    Position start(selectedPiece->objectName()[1] - '0', selectedPiece->objectName()[3] - '0');
    Position end(destination->objectName()[1] - '0', destination->objectName()[3] - '0');

    // Make move and update accordingly
    if (gameController_->makeMove(start, end)) {
        updateBoard();
    }
}

/**
 * @brief Determines if the game state is convertible to a normal chess game.
 * @param currentColour Colour of who to initialize play as.
 * @return True if game state is valid, False otherwise.
*/
bool TestBoardUI::validateGameState(Colour currentColour) {
    // If the other player is in check on the current player's turn, game state is invalid.
    bool otherCheck;
    
    if(currentColour == gameController_->getCurrentPlayer()->getColour()) {
        gameController_->switchTurns();
        otherCheck = gameController_->isKingInCheck();
        gameController_->switchTurns();
    } else {
        otherCheck = gameController_->isKingInCheck();
    }

    return !otherCheck; 
}