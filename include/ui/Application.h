/**
 * @file Application.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the Application.
 * @date 2023-10-14
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WWidget.h>

#include "Login.h"
#include "CreateAccount.h"
#include "Homepage.h"
#include "UserStatistics.h"
#include "ChessBoardUI.h"
#include "User.h"
#include "Player.h"
#include "GameController.h"

class Application : public Wt::WApplication {
public:
    Application(const Wt::WEnvironment& env);
    ~Application();
    void initializeComponents();
    void initializeSubpages();

private:
    Wt::WStackedWidget *container_; /** Main container to hold subpages */

    /** Application Subpages */
    Login *userOneLogin_; /** Logs in the first user -> user1_ */
    Login *userTwoLogin_; /** Logs in the second user -> user2_ */
    CreateAccount *createAccount_; /** Account creation */
    Homepage *homepage_; /** Homepage of the application */

    /** Application Components */
    UserStatistics *userOneStatistics_; /** Statistics for the first user -> user1_ */
    UserStatistics *userTwoStatistics_; /** Statistics for the second user -> user2_ */

    /** User instances to persist throughout the application */
    User *user1_;
    User *user2_;

    /** Game Controller instance */
    GameController *gameController_;

    /** ChessBoardUI Instance*/
    ChessBoardUI *chessBoardUI_;
};

#endif // APPLICATION_H
