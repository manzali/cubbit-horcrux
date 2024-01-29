#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>

#include "endpoint.hpp"
#include "dataformat.hpp"
#include "args.hpp"
#include "management.hpp"
#include "request.hpp"

int main(int argc, char *argv[])
{

  try
  {

    horcrux::args::client_args const args = horcrux::args::parse_client_args(argc, argv);

    std::cout << "req: " << args.req << std::endl;
    std::cout << "n_chunk: " << args.n_chunks << std::endl;
    std::cout << "file_path: " << args.file_path << std::endl;
    std::cout << "file_id: " << args.file_id << std::endl;

    std::string uuid = horcrux::management::generate_uuid();

    {
      auto horcruxes{horcrux::management::generate_horcruxes_from_file(args.file_path, args.n_chunks)};

      auto sr = horcrux::request::generate_save_request(uuid, horcruxes.size(), 0, horcruxes[0]);
      std::cout << "save request:\n"
                << bj::serialize(sr) << std::endl;

      horcrux::management::save_horcruxes_to_disk(horcruxes, uuid, "C:\\Temp");
    }

    {
      auto horcruxes{horcrux::management::load_horcruxes_from_disk(uuid, "C:\\Temp")};
      horcrux::management::generate_file_from_horcruxes("C:\\Temp\\tree2.jpg", horcruxes);
    }
  }
  catch (std::runtime_error &e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  /*
    auto const ep = horcrux::endpoint::make_endpoint("127.0.0.1", "8282");

    using boost::asio::ip::tcp;
    boost::asio::io_context io_context;

    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "44124"));

    std::string data{"some client data ..." + horcrux::dataformat::msg_delimiter};
    auto result = boost::asio::write(socket, boost::asio::buffer(data));

    std::cout << "data sent: " << data.length() << '/' << result << std::endl;

    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket.close();
  */
  return 0;
}