/**
 * @file Homepage.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @author Satvir Singh Uppal (suppal46@uwo.ca)
 * @brief This is the header file for the Homepage UI.
 * @date 2023-10-14
 */
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>

#include "UserStatistics.h"
#include "User.h"
#include "GameController.h"
#include "ChessBoardUI.h"
#include "LoadGame.h"
#include "WStackedWidget.h"

class Homepage : public Wt::WObject {
public:
    Homepage(WStackedWidget* container, UserStatistics* userOneStatistics, UserStatistics* userTwoStatistics, User* user1, User* user2, GameController* gameController, ChessBoardUI* chessBoardUI);
    ~Homepage();
    
    void manageLoginButtons(int player, bool login); 
    void testToPlay(std::string boardString, Colour colour); /** Transition from test board to game board */
    void showLoadableGame();
    void setLoadpage(LoadGame* loadGame); /** Set the load game page */

private:
    Wt::WText *title_;
    Wt::WPushButton *playButton_; /** Play a game of Chess button */
    Wt::WPushButton *manageUserOne_; /** Logout user one button */
    Wt::WPushButton *manageUserTwo_; /** Logout user two button */
    Wt::WPushButton *testModeButton_; /** Enter test mode button */

    Wt::WPushButton *loadGameButton_; /** Button to load the current game */

    Wt::WContainerWidget *homepageContainer_;
    Wt::WGridLayout *homepageLayout_;
    WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */

    /** User instances to reset upon logging out */
    User *user1_;
    User *user2_;

    /** User statistics instances to update */
    UserStatistics *userOneStatistics_;
    UserStatistics *userTwoStatistics_;

    LoadGame *loadGame_; /** Load game page */

    ChessBoardUI *chessBoardUI_; /** ChessBoardUI instance */

    /** Game Controller instance */
    GameController *gameController_;

    void navigateToPlay(); /** Navigate to play a game of chess */
    void manageUserOne(); /** Logout/login user one */
    void manageUserTwo(); /** Logout/login user two */

    void loadGamePage(); /** Handle navigation to Load Game page */
};

#endif // HOMEPAGE_H
