/**
 * @file TestBoardUI.h
 * @author Chris Hosang
 * @brief This is the header file for the test board page.
 * @date 2023-11-14
 */

#ifndef TEST_BOARD_UI_H
#define TEST_BOARD_UI_H

#include "ChessBoardUI.h"
#include "PieceFactory.h"
#include "Homepage.h"
#include "WStackedWidget.h"

class TestBoardUI : public ChessBoardUI {
    public:
        TestBoardUI(WStackedWidget *container, GameController *gameController);
        ~TestBoardUI();

        void initializeBoard(); /** Initializes chess board */
        void startTest(); /** Initializes test board environment */
        void setHomepage(Homepage *homepage); /** Sets the homepage reference */

    private:
        Homepage *homepage_; /** Homepage reference */
        std::string pieceToMake_; /** Piece to place on board */
        std::unordered_map<std::string, Wt::WPushButton*> pieceMaker_; /** Piece maker buttons */
        Wt::WContainerWidget *makerContainer_; /** Container holding piece maker */
        Wt::WGridLayout *makerLayout_; /** Grid layout for piece maker */
        Wt::WPushButton *deleteButton_; /** Delete piece button */
        Wt::WPushButton *playAsWhite_; /** Play as white button */
        Wt::WPushButton *playAsBlack_; /** Play as black button*/

        void initializePieceMaker(); /** Initializes piece maker buttons */
        void handleButtonPress(Wt::WPushButton* btn); /** Handles chess board input */
        void handleMakerClicked(std::string name, Wt::WPushButton* btn); /** Behaviour for when piece maker button is clicked */
        void updateMakerStyle(); /** Resets piece maker button styles */
        void makeMove(Wt::WPushButton* selectedPiece, Wt::WPushButton* destination); /** Moves a piece */
        void handlePlayButton(Colour colour); /** Behaviour for when play as black or white buttons are clicked */
        bool validateGameState(Colour currentColour); /** Validates that a game state is convertible to a real game */
};

#endif // TEST_BOARD_UI_H