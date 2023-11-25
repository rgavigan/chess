/**
 * @file LoadGame.h
 * @author Satvir Singh Uppal (suppal46@uwo.ca)
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the LoadGame UI.
 * @date 2023-01-11
 */

#ifndef LOADGAME_H
#define LOADGAME_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>
#include <vector>
#include <utility>
#include <unordered_map>

#include "User.h"
#include "GameController.h"
#include "WStackedWidget.h"
#include "ChessBoardUI.h"

class LoadGame : public Wt::WObject {

public:
    LoadGame(WStackedWidget* container, GameController* gameController, User* user1, User* user2, ChessBoardUI* chessBoardUI); 
    ~LoadGame();
    void renderLoadableGames(); /** Renders the loadable games for the current users */

private:
    WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */
    GameController *gameController_; /** Game controller instance */
    User *user1_; /** User one instance */
    User *user2_; /** User two instance */
    ChessBoardUI *chessBoardUI_; /** ChessBoardUI instance */

    Wt::WText *title_; /** Title of the load game page */
    Wt::WText *errorText_; /** Error text to display if there are no loadable games */

    Wt::WGridLayout *loadGameLayout_; /** Layout for the load game page */
    Wt::WContainerWidget *loadGameContainer_; /** Container for the load game page */
    Wt::WContainerWidget *loadableGames_; /** Container for the loadable games */
    Wt::WGridLayout * loadableGamesLayout_; /** Layout for the loadable games */
    Wt::WPushButton *homepageButton_; /** Button to navigate back to homepage */
    Wt::Signal<int> *gameSelected_; /** Signal to emit when a game is selected */

    void backToHomepage(); /** Navigates back to homepage */
};

#endif // LOADGAME_H