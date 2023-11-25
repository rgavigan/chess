/**
 * @file UserStatistics.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the user statistics component.
 * @date 2023-10-14
 */
#ifndef USER_STATISTICS_H
#define USER_STATISTICS_H

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>
#include <string>
#include <chrono>
#include <sstream>
#include <cmath>

#include "User.h"
#include "Player.h"

class UserStatistics : public Wt::WObject {
public:
    UserStatistics(User* user);
    ~UserStatistics();
    std::unique_ptr<Wt::WContainerWidget> getStatisticsContainer();
    std::unique_ptr<Wt::WContainerWidget> getGameStatisticsContainer();
    void updateStatistics(Player *player);
    void setEloDiff(std::pair<double, double> eloChanges);

private:
    /** Homepage statistics */
    Wt::WText *userInfo_; /** Information: username, win-loss-draw, elo */
    User* user_; /** User instance to retrieve statistics from */
    Wt::WComboBox *timerComboBox_; /** Timer combo box to select timer */
    std::unique_ptr<Wt::WContainerWidget> statisticsContainer_;
    Wt::WGridLayout *statisticsLayout_;

    /** Game statistics */
    Wt::WText *gameUserInfo_; /** Information: username, win-loss-draw, elo */
    Wt::WText *gameUserTimer_; /** Information: timer */
    std::unique_ptr<Wt::WContainerWidget> gameStatisticsContainer_;
    Wt::WGridLayout *gameStatisticsLayout_;

    std::string eloDiff_ = ""; /** Elo gained or lost after the current match */

    void updateTimer(); /** Update the timer for the user */
};

#endif // USER_STATISTICS_H
