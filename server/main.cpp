#include <iostream>

#include "horcrux/endpoint.hpp"
#include "horcrux/server.hpp"

#define HOST "127.0.0.1"
#define PORT "44124"
#define SERVER_DIR "C:\\Temp"

int main(int argc, char *argv[])
{

  try
  {
    auto const ep = horcrux::endpoint::make_endpoint(HOST, PORT);
    horcrux::server::server s(ep, SERVER_DIR);
    s.start();
  }
  catch (std::runtime_error &e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  return 0;
}