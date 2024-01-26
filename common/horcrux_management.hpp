#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>

namespace fs = std::filesystem;

namespace horcrux {

namespace management {

typedef std::vector<char> horcrux;

std::string generate_uuid()
{
  return boost::lexical_cast<std::string>(boost::uuids::random_generator()());
}

std::vector<horcrux> generate_horcrux_from_file(fs::path file_path, unsigned int n_chunks)
{
  std::ifstream ifs(file_path, std::ios::binary);
  ifs.seekg(0, std::ios::end);
  size_t const file_size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  size_t const chunk_size = file_size / n_chunks;
  size_t remaining_size = file_size;

  std::vector<horcrux> horcrux_collection;

  while(remaining_size > 0)
  {
    size_t const current_chunk_size = (remaining_size > chunk_size ? chunk_size : remaining_size - chunk_size);
    remaining_size -= current_chunk_size;

    horcrux h(current_chunk_size);
    ifs.read(h.data(), current_chunk_size);
    horcrux_collection.push_back(std::move(h));
  }

  ifs.close();
  return horcrux_collection;
}

}

}