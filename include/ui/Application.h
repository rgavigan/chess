/**
 * @file Application.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the Application.
 * @date 2023-10-14
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include <Wt/WApplication.h>
#include <Wt/WWidget.h>

#include "WStackedWidget.h"
#include "Login.h"
#include "CreateAccount.h"
#include "Homepage.h"
#include "UserStatistics.h"
#include "ChessBoardUI.h"
#include "TestBoardUI.h"
#include "User.h"
#include "Player.h"
#include "GameController.h"
#include "LoadGame.h"

class Application : public Wt::WApplication {
public:
    Application(const Wt::WEnvironment& env);
    ~Application();
    void initializeComponents();
    void initializeSubpages();
    void handleSignal();

private:
    WStackedWidget *container_; /** Main container to hold subpages */

    /** Application Subpages */
    Login *userOneLogin_; /** Logs in the first user -> user1_ */
    Login *userTwoLogin_; /** Logs in the second user -> user2_ */
    CreateAccount *createAccount_; /** Account creation */
    Homepage *homepage_; /** Homepage of the application */
    LoadGame *loadGame_; /** Load Game page of the application*/

    /** Application Components */
    UserStatistics *userOneStatistics_; /** Statistics for the first user -> user1_ */
    UserStatistics *userTwoStatistics_; /** Statistics for the second user -> user2_ */

    /** User instances to persist throughout the application */
    User *user1_;
    User *user2_;

    /** Game Controller instance */
    GameController *gameController_;

    /** Test Game Controller instance */
    GameController *testController_;

    /** ChessBoardUI Instance*/
    ChessBoardUI *chessBoardUI_;

    /** TestBoardUI Instance*/
    TestBoardUI *testBoardUI_;
};

#endif // APPLICATION_H
