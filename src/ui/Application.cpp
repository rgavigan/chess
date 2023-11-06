/**
 * @file Application.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the main Application class.
 * This class is responsible for creating the main container that holds all other 
 * sub-containers.
 * @date 2023-10-14
 */
#include <Wt/WApplication.h>
#include "Application.h"

/**
 * @brief Construct a new PiChess Application
 * @date 2023-10-14
 * @param env Wt environment
 */
Application::Application(const Wt::WEnvironment& env) : WApplication(env) {
    setTitle("PiChess");
    useStyleSheet("src/ui/style.css");

    // Initialize container to hold subpages 
    container_ = root()->addWidget(std::make_unique<Wt::WStackedWidget>());

    // Initialize empty users to allow modification of the instance 
    std::string username = "";
    user1_ = new User(username);
    user2_ = new User(username);

    // Initialize players based on user1 and user2 
    std::unique_ptr<Player> whitePlayer = std::make_unique<Player>(*user1_, Colour::WHITE, std::chrono::minutes(5));
    std::unique_ptr<Player> blackPlayer = std::make_unique<Player>(*user2_, Colour::BLACK, std::chrono::minutes(5));

    // Initialize game controller instance 
    gameController_ = new GameController(std::move(whitePlayer), std::move(blackPlayer), "db/default.sql");

    // Initialize components 
    initializeComponents();

    // Initialize subpages 
    initializeSubpages();

    // Launch application with the homepage 
    container_->setCurrentIndex(5);
}

/**
 * @brief Destroy the Application object
 * @date 2023-10-14
 */
Application::~Application() {}

/**
 * @brief Initialize the components of the application
 * @date 2023-10-15
 */
void Application::initializeComponents() {
    userOneStatistics_ = new UserStatistics(user1_);
    userTwoStatistics_ = new UserStatistics(user2_);
}

/**
 * @brief Initialize the subpages of the application
 * @date 2023-10-15
 */
void Application::initializeSubpages() {
    userOneLogin_ = new Login(container_, user1_, userOneStatistics_, user2_, 1); // Index 0 
    createAccount_ = new CreateAccount(container_); // Index 1 
    chessBoardUI_ = new ChessBoardUI(container_, gameController_, user1_, user2_, userOneStatistics_, userTwoStatistics_); // Index 2
    userTwoLogin_ = new Login(container_, user2_, userTwoStatistics_, user1_, 2); // Index 3 
    createAccount_ = new CreateAccount(container_); // Index 4 
    homepage_ = new Homepage(container_, userOneStatistics_, userTwoStatistics_, user1_, user2_, gameController_, chessBoardUI_); // Index 5

    // Setup page references
    userOneLogin_->setHomepage(homepage_);
    userTwoLogin_->setHomepage(homepage_);
}
