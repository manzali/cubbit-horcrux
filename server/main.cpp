#include <iostream>

#include "endpoint.hpp"
#include "horcrux_server.hpp"

int main(int argc, char* argv[]) {

  try {

    // create engine with 1 thread dedicated
    //common::engine::engine ios(1);

    auto const ep = horcrux::endpoint::make_endpoint("127.0.0.1", "44124");

    boost::asio::io_context io_context;
    horcrux_server s(io_context, ep);
    io_context.run();


    std::cout << "Press enter to close the server...";
    char ch = getchar();

    //ios.stop();

  } catch (std::runtime_error& e) {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  return 0;
}