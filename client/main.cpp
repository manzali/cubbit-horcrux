#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "endpoint.hpp"
#include "dataformat.hpp"

namespace fs = std::filesystem;
namespace po = boost::program_options;

namespace
{
  struct client_args
  {
    std::string cmd;
    unsigned int n_chunks;
    fs::path file_path;
  };

  client_args parse_client_args(int argc, char* argv[])
  {
    po::options_description desc("Invocation : <program> <command> -n <chunks> <input_file>");
    client_args args;

    po::positional_options_description p;
    p.add("command", 1);
    p.add("input", 1);

    desc.add_options()
      ("help,h", "print help messages.")
      ("command,c", po::value<std::string>(&args.cmd)->required(), "command [save|load].")
      ("num,n", po::value<unsigned int>(&args.n_chunks)->required(), "number of chunks in which the file will be splitted.")
      ("input,i", po::value<fs::path>(&args.file_path)->required(), "path to the file that will be sent.");

    try {

      po::variables_map vm;
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

      if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
      }

      po::notify(vm);

      // Check if input file exists
      if (!fs::exists(args.file_path))
      {
        std::cerr << "Missing input file " << args.file_path << std::endl;
        exit(EXIT_FAILURE);
      }

      // check if the number of chunk is greater then the file size
      unsigned int const file_size = fs::file_size(args.file_path);
      if(args.n_chunks > file_size)
      {
        std::cerr << "Number of chunks (" << args.n_chunks << ") cannot be greater than file size in bytes (" << file_size << ")" << std::endl;
        exit(EXIT_FAILURE);
      }

    } catch (po::error const &e) {
      std::cerr << e.what() << '\n' << desc << std::endl;
      exit(EXIT_FAILURE);
    }

    return args;
  }
}


int main(int argc, char* argv[])
{

  // parse command line options (it doesn't raise an exception)
  client_args const args = parse_client_args(argc, argv);

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