#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>

#include "endpoint.hpp"
#include "dataformat.hpp"
#include "args_parser.hpp"
#include "horcrux_management.hpp"

int main(int argc, char* argv[])
{

  horcrux::client_args const args = horcrux::parse_client_args(argc, argv);


  std::string uuid = horcrux::management::generate_uuid();

  std::cout << "uuid : " << uuid << std::endl;

  auto horcrux_collection { horcrux::management::generate_horcrux_from_file(args.file_path, args.n_chunks) };












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

  return 0;
}