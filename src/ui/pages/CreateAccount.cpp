/**
 * @file CreateAccount.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the UI class for creating a new account.
 * This class is responsible for calling the back-end to create a new account. It also handles navigation back
 * to the login page.
 * @date 2023-10-14
 */
#include "CreateAccount.h"

/**
 * @brief Construct a new Create Account UI
 * @param container parent Application container
 */
CreateAccount::CreateAccount(WStackedWidget* container) {
    parentContainer_ = container; // Copy parent container reference 

    // Create the container and layout for the account creation form 
    createAccountContainer_ = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    createAccountContainer_->setStyleClass("centered-container");
    createAccountLayout_ = createAccountContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // Title 
    title_ = createAccountLayout_->addWidget(std::make_unique<Wt::WText>("Create New Account"), 0, 0);
    title_->setStyleClass("title");

    // Username input field 
    usernameInput_ = createAccountLayout_->addWidget(std::make_unique<Wt::WLineEdit>(), 1, 0);
    usernameInput_->setPlaceholderText("Username");
    usernameInput_->setStyleClass("wt-wlineedit");
    usernameInput_->enterPressed().connect(this, &CreateAccount::createAccount);

    // Password input field 
    passwordInput_ = createAccountLayout_->addWidget(std::make_unique<Wt::WLineEdit>(), 2, 0);
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setStyleClass("wt-wlineedit");
    passwordInput_->enterPressed().connect(this, &CreateAccount::createAccount);
    passwordInput_->setEchoMode(Wt::EchoMode::Password);

    // Confirm password input field 
    confirmPasswordInput_ = createAccountLayout_->addWidget(std::make_unique<Wt::WLineEdit>(), 3, 0);
    confirmPasswordInput_->setPlaceholderText("Confirm Password");
    confirmPasswordInput_->setStyleClass("wt-wlineedit");
    confirmPasswordInput_->enterPressed().connect(this, &CreateAccount::createAccount);
    confirmPasswordInput_->setEchoMode(Wt::EchoMode::Password);

    // Button to submit create account request 
    createAccountButton_ = createAccountLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 4, 0);
    createAccountButton_->setText("Create Account");
    createAccountButton_->setStyleClass("button");
    createAccountButton_->clicked().connect(this, &CreateAccount::createAccount);

    // Button to navigate back to the login page 
    loginButton_ = createAccountLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 5, 0);
    loginButton_->setText("Already Registered? Login");
    loginButton_->setStyleClass("text-button");
    loginButton_->clicked().connect(this, &CreateAccount::navigateToLogin);

    // Error text 
    errorText_ = createAccountLayout_->addWidget(std::make_unique<Wt::WText>(""), 6, 0);
    errorText_->setStyleClass("error-text");
}

/**
 * @brief Destroy the Create Account UI object
 * @date 2023-10-14
 */
CreateAccount::~CreateAccount() {}

/**
 * @brief Create a new account in the application
 * @date 2023-10-14
 */
void CreateAccount::createAccount() {
    // Retrieve entered username, password, and confirm password 
    std::string username = usernameInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();
    std::string confirmPassword = confirmPasswordInput_->text().toUTF8();

    // Ensure that password and confirmPassword are identical 
    if (password != confirmPassword) {
        // Add error text below create account button 
        errorText_->setText("Passwords do not match.");
        return;
    }

    // Create UserManager instance and call createAccount 
    userManager_ = new UserManager("db/default.sql");
    if (!userManager_->createUser(username, password)) {
        // Add error text below create account button 
        errorText_->setText("Username already exists.");

        // Username validity
        if (!std::regex_match(username, std::regex("^[a-zA-Z0-9-_]+$"))) {
            errorText_->setText("Username must only contain a-z, A-Z, 0-9, -, and _");
        }

        // Password validity
        if (!std::regex_match(password, std::regex("^[a-zA-Z0-9-_*!]+$"))) {
            errorText_->setText("Password must only contain a-z, A-Z, 0-9, -, _, *, and !");
        }
        return;
    }

    // Clear username, password, and confirm password input fields 
    usernameInput_->setText("");
    passwordInput_->setText("");
    confirmPasswordInput_->setText("");
    errorText_->setText("");
    
    // Take the user to the login UI if account is successfully created 
    navigateToLogin();
    return;
}

/**
 * @brief Navigate back to the login page.
 * @date 2023-10-14
 */
void CreateAccount::navigateToLogin() {
    // Take the user to the login UI 
    parentContainer_->setCurrentIndex(parentContainer_->currentIndex() - 1, true);
    return;
}
