/**
 * @file CreateAccount.h
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the header file for the Create Account UI.
 * @date 2023-10-14
 */
#ifndef CREATE_ACCOUNT_H
#define CREATE_ACCOUNT_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/WObject.h>
#include <Wt/WText.h>
#include <regex>

#include "UserManager.h"
#include "WStackedWidget.h"

class CreateAccount : public Wt::WObject {
public:
    CreateAccount(WStackedWidget* container);
    ~CreateAccount();

private:
    Wt::WLineEdit *usernameInput_;
    Wt::WLineEdit *passwordInput_;
    Wt::WLineEdit *confirmPasswordInput_;
    Wt::WPushButton *createAccountButton_; /** Create a new account */
    Wt::WPushButton *loginButton_; /** Navigate back to login page */
    Wt::WText *title_;
    Wt::WText *errorText_;

    Wt::WContainerWidget *createAccountContainer_;
    Wt::WGridLayout *createAccountLayout_;
    WStackedWidget *parentContainer_; /** Parent container stored as a class variable for navigation */

    UserManager *userManager_; /** User manager instance to create new account */

    void createAccount(); /** Create a new account */
    void navigateToLogin(); /** Navigate back to login page */
};

#endif // CREATE_ACCOUNT_H
