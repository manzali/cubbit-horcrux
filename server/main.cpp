#include <iostream>

#include <engine.hpp>

int main(int argc, char* argv[]) {

  std::cout << "Hello, World!" << std::endl;

  try {

    // create engine with 1 thread dedicated
    common::engine::engine engine(1);



    std::cout << "Press enter to close the server...";
    char ch = getchar();

    engine.stop();

  } catch (std::runtime_error& e) {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  return 0;
}