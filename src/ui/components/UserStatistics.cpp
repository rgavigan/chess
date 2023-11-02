/**
 * @file UserStatistics.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the implementation file for the user statistics component.
 * @date 2023-10-14
 */
#include "UserStatistics.h"

/**
 * @brief Construct a new User Statistics component
 * @param user object to retrieve statistics from
 */
UserStatistics::UserStatistics(User* user) {
    user_ = user; // Copy user reference 

    // Create the container and layout for the statistics 
    statisticsContainer_ = std::make_unique<Wt::WContainerWidget>();
    statisticsContainer_->setStyleClass("statistics-container");
    statisticsLayout_ = statisticsContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // User's statistics 
    if (user_->getUsername() == "") {
        userInfo_ = statisticsLayout_->addWidget(std::make_unique<Wt::WText>("Guest Player"), 0, 0, 1, 2);
    }
    else {
        userInfo_ = statisticsLayout_->addWidget(std::make_unique<Wt::WText>(user_->getUsername() + ": " + std::to_string(user_->getWins()) + "-" + std::to_string(user_->getLosses()) + "-" + std::to_string(user_->getDraws())), 0, 0, 1, 2);
    }
    userInfo_->setStyleClass("center-text");

    // Timer combo box for user to select game timer 
    statisticsLayout_->addWidget(std::make_unique<Wt::WText>("Match Timer Minutes: "), 1, 0);
    timerComboBox_ = statisticsLayout_->addWidget(std::make_unique<Wt::WComboBox>(), 1, 1);
    timerComboBox_->addItem("10");
    timerComboBox_->addItem("20");
    timerComboBox_->addItem("30");
    timerComboBox_->addItem("45");
    timerComboBox_->addItem("60");
    timerComboBox_->activated().connect(this, &UserStatistics::updateTimer);
    statisticsLayout_->setColumnStretch(1, 1);

    // Game Statistics 
    gameStatisticsContainer_ = std::make_unique<Wt::WContainerWidget>();
    gameStatisticsContainer_->setStyleClass("game-statistics-container");
    gameStatisticsLayout_ = gameStatisticsContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // User's game statistics 
    if (user_->getUsername() == "") {
        gameUserInfo_ = gameStatisticsLayout_->addWidget(std::make_unique<Wt::WText>("Guest Player"), 0, 0);
    }
    else {
        gameUserInfo_ = gameStatisticsLayout_->addWidget(std::make_unique<Wt::WText>(user_->getUsername() + ": " + std::to_string(user_->getWins()) + "-" + std::to_string(user_->getLosses()) + "-" + std::to_string(user_->getDraws())), 0, 0);
    }
    gameUserInfo_->setStyleClass("center-text");

    std::string minutes = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(user_->getTimer()).count()); // Extract minutes
    std::string seconds = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(user_->getTimer()).count() % 60); // Extract seconds (using modulo to get seconds within the minute)
    
    // Pad seconds to make sure it is 2 digits long, add 0 if necessary 
    if (seconds.length() == 1) {
        seconds = "0" + seconds;
    }
    gameUserTimer_ = gameStatisticsLayout_->addWidget(std::make_unique<Wt::WText>("Timer: " + minutes + ":" + seconds), 0, 1);
    gameUserTimer_->setStyleClass("center-text");
}

/**
 * @brief Destroy the UserStatistics object
 * @date 2023-10-14
 */
UserStatistics::~UserStatistics() {}

/**
 * @brief Get the statistics container
 * @date 2023-10-15
 */
std::unique_ptr<Wt::WContainerWidget> UserStatistics::getStatisticsContainer() {
    return std::move(statisticsContainer_);
}

/**
 * @brief Get the game statistics container
 * @date 2023-10-28
 */
std::unique_ptr<Wt::WContainerWidget> UserStatistics::getGameStatisticsContainer() {
    return std::move(gameStatisticsContainer_);
}

/**
 * @brief Update the user statistics
 * @date 2023-10-15
 */
void UserStatistics::updateStatistics(Player *player) {
    if (user_->getUsername() == "") {
        userInfo_->setText("Guest Player");
        gameUserInfo_->setText("Guest Player");
    }
    else {
        userInfo_->setText(user_->getUsername() + "  |  " + std::to_string(user_->getWins()) + "W-" + std::to_string(user_->getLosses()) + "L-" + std::to_string(user_->getDraws()) + "D");
        gameUserInfo_->setText(user_->getUsername() + "  |  " + std::to_string(user_->getWins()) + "W-" + std::to_string(user_->getLosses()) + "L-" + std::to_string(user_->getDraws()) + "D");
    }
    if (player == nullptr) {
        return;
    }
    std::string minutes = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(player->getTimeLeft()).count()); // Extract minutes
    std::string seconds = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(player->getTimeLeft()).count() % 60); // Extract seconds (using modulo to get seconds within the minute)

    // Pad seconds to make sure it is 2 digits long, add 0 if necessary 
    if (seconds.length() == 1) {
        seconds = "0" + seconds;
    }
    gameUserTimer_->setText("Timer: " + minutes + ":" + seconds);
}

/**
 * @brief Update the timer for the user
 * @date 2023-10-28
 */
void UserStatistics::updateTimer() {
    std::string timerString = timerComboBox_->currentText().toUTF8();
    user_->setTimer(std::chrono::minutes(std::stoi(timerString)));
}
