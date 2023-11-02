/**
 * @file main.cpp
 * @author Riley Emma Gavigan (rgavigan@uwo.ca)
 * @brief This is the main file to run the Chess application.
 * @date 2023-10-14
 */
#include <Wt/WApplication.h>
#include "Application.h"

int main(int argc, char **argv)
{
  // Launch application with the main Game UI 
  return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
    return std::make_unique<Application>(env);
  });
}
