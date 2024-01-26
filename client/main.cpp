#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {


  const unsigned int n_chunks = 1;
  const fs::path file_path = "C:\\Temp\\test.txt";

  // Check if input file exists
  if (!fs::exists(file_path))
  {
    std::cerr << "Missing input file " << file_path << std::endl;
    return EXIT_FAILURE;
  }




  return 0;
}