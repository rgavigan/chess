/**
 * @file Login.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the UI class for logging into an existing account.
 * @date 2023-10-14
 */
#include "Login.h"

/**
 * @brief Construct a new Login UI
 * @param container parent Application container
 * @param user user
 * @param userStatistics user statistics component
 * @param otherUser other user
 */
Login::Login(Wt::WStackedWidget* container, User* user, UserStatistics* userStatistics, User* otherUser) {
    parentContainer_ = container; // Copy parent container reference 

    user_ = user;
    otherUser_ = otherUser;
    userStatistics_ = userStatistics;

    // Create the container and layout for the login form 
    loginContainer_ = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    loginContainer_->setStyleClass("centered-container");
    loginLayout_ = loginContainer_->setLayout(std::make_unique<Wt::WGridLayout>());

    // Title of the login form 
    title_ = loginLayout_->addWidget(std::make_unique<Wt::WText>("PiChess"), 0, 0);
    title_->setStyleClass("title");

    // Username input field for logging in 
    usernameInput_ = loginLayout_->addWidget(std::make_unique<Wt::WLineEdit>(), 1, 0);
    usernameInput_->setPlaceholderText("Username");
    usernameInput_->setStyleClass("wt-wlineedit");

    // Password input field for logging in 
    passwordInput_ = loginLayout_->addWidget(std::make_unique<Wt::WLineEdit>(), 2, 0);
    passwordInput_->setPlaceholderText("Password");
    passwordInput_->setStyleClass("wt-wlineedit");

    // Button to submit login request to the back-end 
    loginButton_ = loginLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 3, 0);
    loginButton_->setText("Login");
    loginButton_->setStyleClass("button");
    loginButton_->clicked().connect(this, &Login::login);

    // Button to navigate to the account creation page if necessary 
    createAccountButton_ = loginLayout_->addWidget(std::make_unique<Wt::WPushButton>(), 4, 0);
    createAccountButton_->setText("Not Registered? Create an Account");
    createAccountButton_->setStyleClass("text-button");
    createAccountButton_->clicked().connect(this, &Login::navigateToCreateAccount);

    // Error text 
    errorText_ = loginLayout_->addWidget(std::make_unique<Wt::WText>(""), 5, 0);
    errorText_->setStyleClass("error-text");
}

/**
 * @brief Destroy the Login UI object
 * @date 2023-10-14
 */
Login::~Login() {}

/**
 * @brief Login to an existing account by calling the back-end, performing validation checks.
 * @date 2023-10-14
 */
void Login::login() {
    // Retrieve entered username and password 
    std::string username = usernameInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();

    // Create UserManager instance and authenticate login 
    userManager_ = new UserManager("db/default.sql");
    if (!userManager_->authenticateUser(username, password)) {
        // Add error text below create account button 
        errorText_->setText("Invalid username or password.");
        return;
    }
    // Handle duplicate user login 
    if (otherUser_->getUsername() == username) {
        // Add error text below create account button 
        errorText_->setText("User already logged in as another player.");
        return;
    }
    // Initialize User object for user_ 
    user_->setUsername(username);

    // Initialize user statistics 
    int wins = 0;
    int losses = 0;
    int draws = 0;
    if (!userManager_->getUserStats(username, wins, losses, draws)) {
        return;
    }
    user_->initializeStats(wins, losses, draws);

    // Clear user and password input fields 
    usernameInput_->setText("");
    passwordInput_->setText("");
    errorText_->setText("");

    // Update user statistics component 
    userStatistics_->updateStatistics(nullptr);

    // Navigate to the home page for the chess application 
    parentContainer_->setCurrentIndex(5);
}

/**
 * @brief Navigate to the create account page.
 * @date 2023-10-14
 */
void Login::navigateToCreateAccount() {
    // Set the current widget to the create account UI
    parentContainer_->setCurrentIndex(parentContainer_->currentIndex() + 1);
}
