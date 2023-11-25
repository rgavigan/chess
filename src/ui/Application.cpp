/**
 * @file Application.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the main Application class.
 * This class is responsible for creating the main container that holds all other 
 * sub-containers.
 * @date 2023-10-14
 */
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
    container_ = root()->addWidget(std::make_unique<WStackedWidget>());

    // Connect signal
    container_->indexChanged().connect(this, &Application::handleSignal);

    // Initialize empty users to allow modification of the instance 
    std::string username = "";
    user1_ = new User(username);
    user2_ = new User(username);

    // Initialize players based on user1 and user2 
    std::unique_ptr<Player> whitePlayer = std::make_unique<Player>(*user1_, Colour::WHITE, std::chrono::minutes(5));
    std::unique_ptr<Player> blackPlayer = std::make_unique<Player>(*user2_, Colour::BLACK, std::chrono::minutes(5));

    std::unique_ptr<Player> testWhitePlayer = std::make_unique<Player>(*user1_, Colour::WHITE, std::chrono::minutes(60));
    std::unique_ptr<Player> testBlackPlayer = std::make_unique<Player>(*user2_, Colour::BLACK, std::chrono::minutes(60));

    // Initialize game controller instance 
    gameController_ = new GameController(std::move(whitePlayer), std::move(blackPlayer), "db/default.sql");

    // Initialize test game controller instance 
    testController_ = new GameController(std::move(testWhitePlayer), std::move(testBlackPlayer), "db/default.sql", true);

    // Initialize components 
    initializeComponents();

    // Initialize subpages 
    initializeSubpages();

    // Launch application with the homepage 
    container_->setCurrentIndex(5, true);
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
    chessBoardUI_ = new ChessBoardUI(container_, gameController_); // Index 2
    userTwoLogin_ = new Login(container_, user2_, userTwoStatistics_, user1_, 2); // Index 3 
    createAccount_ = new CreateAccount(container_); // Index 4 
    homepage_ = new Homepage(container_, userOneStatistics_, userTwoStatistics_, user1_, user2_, gameController_, chessBoardUI_); // Index 5
    testBoardUI_ = new TestBoardUI(container_, testController_); // Index 6
    loadGame_ = new LoadGame(container_, gameController_, user1_, user2_, chessBoardUI_); // Index 7 for the LoadGame Page

    // Setup pages
    chessBoardUI_->initializeComponents(user1_, user2_, userOneStatistics_, userTwoStatistics_);

    // Setup page references
    testBoardUI_->setHomepage(homepage_);
    userOneLogin_->setHomepage(homepage_);
    userTwoLogin_->setHomepage(homepage_); 
    homepage_->setLoadpage(loadGame_);
}

/**
 * @brief Handles signal from stacked widget index change
 * General signals that always need to be called when switching pages
 */
void Application::handleSignal() {
    // Homepage
    if (container_->currentIndex() == 5) {
        homepage_->showLoadableGame();
    }
    // Load Game Page
    else if (container_->currentIndex() == 7) {
        loadGame_->renderLoadableGames();
    }
    // Chess Board Pages
    else if (container_->currentIndex() == 2 || container_->currentIndex() == 6) {
        chessBoardUI_->resetDefaultSquares();
    }
}
