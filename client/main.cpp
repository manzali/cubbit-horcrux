#include <iostream>

#include "horcrux/args.hpp"
#include "horcrux/client.hpp"
#include "horcrux/request.hpp"

#define HOST "127.0.0.1"
#define PORT "44124"

int main(int argc, char *argv[])
{

  try
  {

    horcrux::args::client_args const args = horcrux::args::parse_client_args(argc, argv);
    /*
        std::cout << "req: " << args.req << std::endl;
        std::cout << "n_chunk: " << args.n_chunks << std::endl;
        std::cout << "file_path: " << args.file_path << std::endl;
        std::cout << "uuid: " << args.uuid << std::endl;
    */

    horcrux::client::client c;

    c.connect(HOST, PORT);

    int ret = -1;

    switch (args.req)
    {
    case horcrux::request::request_type::SAVE:
      ret = c.send_save_request(args.file_path, args.n_chunks);
      break;

    case horcrux::request::request_type::LOAD:
      ret = c.send_load_request(args.uuid, args.file_path);
      break;

    default:
      break;
    }

    if (ret == horcrux::dataformat::status_code::SAVE_REQUEST_OK || horcrux::dataformat::status_code::LOAD_REQUEST_OK)
    {
      std::cout << "Request handled successfully" << std::endl;
    }
    else
    {
      std::cout << "Request handled with error: " << ret << std::endl;
    }

    c.disconnect();
  }
  catch (std::runtime_error &e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  return 0;
}