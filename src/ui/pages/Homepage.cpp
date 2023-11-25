/**
 * @file Homepage.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @author Satvir Singh Uppal (suppal46@uwo.ca)
 * @brief This is the UI class for the homepage after logging in.
 * @date 2023-10-14
 */
#include "Homepage.h"

/**
 * @brief Construct a new Homepage UI
 * @param container parent Application container
 * @param userOneStatistics user one statistics component
 * @param userTwoStatistics user two statistics component
 * @param user1 user one
 * @param user2 user two
 * @param gameController game controller
 */
Homepage::Homepage(WStackedWidget* container, UserStatistics* userOneStatistics, UserStatistics* userTwoStatistics, User* user1, User* user2, GameController* gameController, ChessBoardUI* chessBoardUI) {
    parentContainer_ = container; // Copy parent container reference 
    user1_ = user1; // Copy user1 reference 
    user2_ = user2; // Copy user2 reference 
    userOneStatistics_ = userOneStatistics; // Copy user one statistics reference 
    userTwoStatistics_ = userTwoStatistics; // Copy user two statistics reference 
    gameController_ = gameController; // Copy game controller reference 
    chessBoardUI_ = chessBoardUI; // Copy chess board UI reference

    // Create the container and layout for the homepage 
    homepageContainer_ = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    homepageContainer_->setStyleClass("centered-container");
    homepageLayout_ = homepageContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // Title of the homepage 
    title_ = homepageLayout_->addWidget(std::make_unique<Wt::WText>("PiChess Home"), 0, 0, 1, 2);
    title_->setStyleClass("title");

    // Button to navigate to play a game of chess 
    playButton_ = homepageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 1, 0, 1, 2);
    playButton_->setText("Play Game");
    playButton_->setStyleClass("button");
    playButton_->clicked().connect(this, &Homepage::navigateToPlay);

    // User one statistics component 
    homepageLayout_->addWidget(userOneStatistics->getStatisticsContainer(), 2, 0);

    // Login/logout user one button 
    manageUserOne_ = homepageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 3, 0);
    manageUserOne_->setText("Login Player 1");
    manageUserOne_->setStyleClass("login-button");
    manageUserOne_->clicked().connect(this, &Homepage::manageUserOne);

    // User two statistics component 
    homepageLayout_->addWidget(userTwoStatistics->getStatisticsContainer(), 2, 1);
    
    // Login/logout user two button 
    manageUserTwo_ = homepageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 3, 1);
    manageUserTwo_->setText("Login Player 2");
    manageUserTwo_->setStyleClass("login-button");
    manageUserTwo_->clicked().connect(this, &Homepage::manageUserTwo);

    // Enter test mode button
    testModeButton_ = homepageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 4, 0, 1, 2);
    testModeButton_->setText("Enter Test Mode");
    testModeButton_->setStyleClass("text-button");
    testModeButton_->clicked().connect([=] {
        parentContainer_->setCurrentIndex(6, true);
    });
    
    loadGameButton_ = homepageLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 5, 0, 1, 2);
    loadGameButton_->setStyleClass("red-button");
    loadGameButton_->setText("Load Game");
    loadGameButton_->clicked().connect([this] {loadGamePage();});
    showLoadableGame();
}

/**
 * @brief Destroy the Homepage UI object
 * @date 2023-10-14
 */
Homepage::~Homepage() {}

/**
 * @brief Navigate to play a game of chess
 * @date 2023-10-14
 */
void Homepage::navigateToPlay() {
    // Start the game in ChessBoardUI
    chessBoardUI_->startGame();
    parentContainer_->setCurrentIndex(2, true);
}

/**
 * @brief Manage text and styling of the login buttons
 * @param player Player 1 or 2
 * @param login True if user is logging in, False if no login occurs (user logs out or back button is pressed)
*/
void Homepage::manageLoginButtons(int player, bool login) {
    // Player 1
    if(player == 1) {
        if(login) { // Successful login
            manageUserOne_->setText("Logout Player 1");
            manageUserOne_->setStyleClass("logout-button");
        } else {
            manageUserOne_->setText("Login Player 1");
            manageUserOne_->setStyleClass("login-button");
        }
    } 
    // Player 2
    else {
        if(login) { // Successful login
            manageUserTwo_->setText("Logout Player 2");
            manageUserTwo_->setStyleClass("logout-button");
        } else {
            manageUserTwo_->setText("Login Player 2");
            manageUserTwo_->setStyleClass("login-button");
        }
    }
    showLoadableGame();
}

/**
 * @brief Manage login/logout of user one
 * @date 2023-10-15
 */
void Homepage::manageUserOne() {
    // Login condition 
    if (user1_->getUsername() == "") {
        parentContainer_->setCurrentIndex(0, true);
        manageLoginButtons(1, true);
        manageUserOne_->setText("Logout Player 1");
        manageUserOne_->setStyleClass("logout-button");
    }
    // Logout condition 
    else {
        user1_->resetUser();
        manageLoginButtons(1, false);
    }
    // Update user statistics component 
    userOneStatistics_->updateStatistics(nullptr);
    showLoadableGame();
}

/**
 * @brief Manage login/logout of user two
 * @date 2023-10-15
 */
void Homepage::manageUserTwo() {
    // Login condition 
    if (user2_->getUsername() == "") {
        parentContainer_->setCurrentIndex(3, true);
        manageLoginButtons(2, true);

        manageUserTwo_->setText("Logout Player 2");
        manageUserTwo_->setStyleClass("logout-button");
    }
    // Logout condition 
    else { 
        user2_->resetUser();
        manageLoginButtons(2, false);
    }
    // Update user statistics component 
    userTwoStatistics_->updateStatistics(nullptr);
    showLoadableGame();
}

/**
 * @brief Transition from test board to game board.
 * @param boardString Test board state, as a string.
 * @param turn Colour of who's turn it is.
*/
void Homepage::testToPlay(std::string boardString, Colour colour) {
    chessBoardUI_->startFromTest(boardString, colour);
}

/**
 * @brief Shows the load game button if there are games to load, hides it otherwise
 * @author Riley Gavigan
*/
void Homepage::showLoadableGame(){
    if(user1_->getUsername() != "" && user2_->getUsername() != "" && gameController_->getNumSaves(user1_, user2_) > 0) {
        loadGameButton_->show();
    }
    else {
        loadGameButton_->hide();
    }
}

/**
 * @brief Handle navigation to load game, stopping the match
 */
void Homepage::loadGamePage(){
    // Navigate to the load page
    parentContainer_->setCurrentIndex(7, true);

    // Render the loadable games
    loadGame_->renderLoadableGames();
}

/**
 * @brief Sets the load game page reference
 * 
 * @param loadGame 
 */
void Homepage::setLoadpage(LoadGame* loadGame){
    loadGame_ = loadGame;
}