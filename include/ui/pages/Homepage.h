/**
 * @file Homepage.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the Homepage UI.
 * @date 2023-10-14
 */
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>

#include "UserStatistics.h"
#include "User.h"
#include "GameController.h"
#include "ChessBoardUI.h"

class Homepage : public Wt::WObject {
public:
    Homepage(Wt::WStackedWidget* container, UserStatistics* userOneStatistics, UserStatistics* userTwoStatistics, User* user1, User* user2, GameController* gameController, ChessBoardUI* chessBoardUI);
    ~Homepage();

private:
    Wt::WText *title_;
    Wt::WPushButton *playButton_; /** Play a game of Chess button */
    Wt::WPushButton *manageUserOne_; /** Logout user one button */
    Wt::WPushButton *manageUserTwo_; /** Logout user two button */

    Wt::WContainerWidget *homepageContainer_;
    Wt::WGridLayout *homepageLayout_;
    Wt::WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */

    /** User instances to reset upon logging out */
    User *user1_;
    User *user2_;

    /** User statistics instances to update */
    UserStatistics *userOneStatistics_;
    UserStatistics *userTwoStatistics_;

    ChessBoardUI *chessBoardUI_; /** ChessBoardUI instance */

    /** Game Controller instance */
    GameController *gameController_;

    void navigateToPlay(); /** Navigate to play a game of chess */
    void manageUserOne(); /** Logout/login user one */
    void manageUserTwo(); /** Logout/login user two */
};

#endif // HOMEPAGE_H
