/**
 * @file LoadGame.h
 * @author Satvir Singh Uppal (suppal46@uwo.ca)
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief LoadGame UI that is a seperate screen and only accessible for users when they are logged in (i.e. both users are logged in at once)
 * @date 2023-01-11
 */

#include "LoadGame.h"

LoadGame::LoadGame(WStackedWidget* container, GameController* gameController, User* user1, User* user2, ChessBoardUI* chessBoardUI) {
    gameController_ = gameController; // Copy game controller reference
    parentContainer_ = container; // Copy parent container reference 
    user1_ = user1; // Copy user1 reference
    user2_ = user2; // Copy user2 reference
    chessBoardUI_ = chessBoardUI; // Copy chess board UI reference

    // Container for the load game page
    loadGameContainer_ = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    loadGameContainer_->setStyleClass("centered-container");
    loadGameLayout_ = loadGameContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // Title for the load game page
    title_ = loadGameLayout_->addWidget(std::make_unique<Wt::WText>("Load a Save"), 0, 0);
    title_->setStyleClass("title");

    // Back to homepage button
    homepageButton_ = loadGameLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 1, 0);
    homepageButton_->setText("Back");
    homepageButton_->setStyleClass("button");
    homepageButton_->clicked().connect(this, &LoadGame::backToHomepage);

    // Container to render the loadable games
    loadableGames_ = loadGameLayout_->addWidget(std::make_unique<Wt::WContainerWidget>(), 2, 0);
    loadableGames_->setStyleClass("centered-container");
    loadableGamesLayout_ = loadableGames_->setLayout(std::make_unique<Wt::WGridLayout>());

    // Error text
    errorText_ = loadGameLayout_->addWidget(std::make_unique<Wt::WText>(""), 3, 0);
    errorText_->setStyleClass("error-text");

    renderLoadableGames();
}

/**
 * @brief Destroy the Load Game:: Load Game object - default behaviour
 * 
 */
LoadGame::~LoadGame() {}

/**
 * @brief Navigates back to the homepage from the load game page
 * 
 */
void LoadGame::backToHomepage() {
    parentContainer_->setCurrentIndex(5, true);
}

/**
 * @brief Renders the loadable games for the current 2 users to the load game page
 */
void LoadGame::renderLoadableGames() {
    // Refresh the loadable games layout
    loadableGamesLayout_ = loadableGames_->setLayout(std::make_unique<Wt::WGridLayout>());
    std::vector<std::unordered_map<std::string, std::string>> games = gameController_->getLoadableGames(user1_, user2_);
    errorText_->setText(""); // Clear error text

    // Display error when no game is loadable
    if (games.size() == 0) {
        errorText_->setText("No loadable games found.");
        return;
    }

    // Loop through all loadable games and display them
    int i = 0;
    for (const auto& game: games) {
        auto gameToLoad = std::make_unique<Wt::WContainerWidget>();
        gameToLoad->setStyleClass("loadable-game");
        gameToLoad->setPadding(5);

        // Retrieve information needed
        int gameID = std::stoi(game.at("gameID"));
        std::string currentPlayer = game.at("currentUser");
        std::string opponentPlayer = game.at("opponentUser");
        std::string currentColour = game.at("currentColour");
        std::string opponentColour = game.at("opponentColour");
        std::string timestamp = game.at("timestamp");
        std::string turnNumber = game.at("turnNumber");
        std::string gameStatus = game.at("gameStatus");
        std::string currentTime = game.at("currentTime");
        std::string opponentTime = game.at("opponentTime");

        // Convert currentTime to integer and then to string with "seconds after:
        int currentTimeInt = std::stoi(currentTime);
        int currentMinutes = currentTimeInt / 60;
        int currentSeconds = currentTimeInt % 60;
        currentTime = std::to_string(currentMinutes) + "m" + std::to_string(currentSeconds) + "s";

        // Convert opponentTime to integer and then to string with "seconds after:
        int opponentTimeInt = std::stoi(opponentTime);
        int opponentMinutes = opponentTimeInt / 60;
        int opponentSeconds = opponentTimeInt % 60;
        opponentTime = std::to_string(opponentMinutes) + "m" + std::to_string(opponentSeconds) + "s";
        
        // Print out the game information
        auto title = gameToLoad->addWidget(std::make_unique<Wt::WText>(
            gameStatus + " - Turn " + turnNumber + " - " + currentColour + 
            " || " + currentPlayer + " (" + currentColour + " - " + currentTime + ")" + " || " + 
            opponentPlayer + " (" + opponentColour + " - " + opponentTime + ")"));
        title->setStyleClass("loadable-game-title");

        // Select button
        auto selectButton = gameToLoad->addWidget(std::make_unique<Wt::WPushButton>("Select"));
        selectButton->clicked().connect([=] {
            // Load the game
            chessBoardUI_->startGameFromID(gameID);

            // Navigate to the game page
            parentContainer_->setCurrentIndex(2, true);
        });
        selectButton->addStyleClass("load-button");

        // Delete button
        auto deleteButton = gameToLoad->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
        deleteButton->clicked().connect([=] {
            // Delete the game
            gameController_->removeGame(gameID);

            // Refresh the loadable games
            renderLoadableGames();
        });
        deleteButton->addStyleClass("delete-button");

        // Add the game to the loadable games layout
        loadableGamesLayout_->addWidget(std::move(gameToLoad), i, 0);
        i++;
    }
}
