/**
 * @file Login.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the Login UI.
 * @date 2023-10-14
 */
#ifndef LOGIN_H
#define LOGIN_H

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>

#include "UserManager.h"
#include "User.h"
#include "UserStatistics.h"

class Login : public Wt::WObject {
public:
    Login(Wt::WStackedWidget* container, User* user, UserStatistics* userStatistics, User* otherUser);
    ~Login();

private:
    Wt::WLineEdit *usernameInput_;
    Wt::WLineEdit *passwordInput_;
    Wt::WPushButton *loginButton_; /** To login to an existing account */
    Wt::WPushButton *createAccountButton_; /** To navigate to create account page */
    Wt::WText *title_;
    Wt::WText *errorText_;

    Wt::WContainerWidget *loginContainer_;
    Wt::WGridLayout *loginLayout_;
    Wt::WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */

    User *user_; /** User instance to be logged into */
    User *otherUser_; /** Other user to check to make sure unique usernames chosen */
    UserManager *userManager_; /** User manager instance to validate login */
    UserStatistics *userStatistics_; /** User statistics component to update on login */

    void login(); /** To login to an existing account */
    void navigateToCreateAccount(); /** To navigate to create account page */
};

#endif // LOGIN_H
