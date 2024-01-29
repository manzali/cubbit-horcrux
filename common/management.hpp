#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/beast/core/detail/base64.hpp>

namespace fs = std::filesystem;
namespace bb = boost::beast::detail::base64;

namespace horcrux
{

  namespace management
  {

    std::string generate_uuid()
    {
      return boost::lexical_cast<std::string>(boost::uuids::random_generator()());
    }

    std::string generate_horcrux_name(std::string uuid, int n)
    {
      return uuid + "_" + std::to_string(n);
    }

    std::vector<std::string> generate_horcruxes_from_file(fs::path file_path, unsigned int n_chunks)
    {
      // Check if file exists
      if (!fs::exists(file_path))
      {
        std::cerr << "Path \"" << file_path << "\" doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      }

      std::ifstream ifs(file_path, std::ios::binary);
      ifs.seekg(0, std::ios::end);
      size_t const file_size = ifs.tellg();
      ifs.seekg(0, std::ios::beg);

      size_t const chunk_size = file_size / n_chunks;
      size_t remaining_size = file_size;

      std::vector<std::string> horcruxes;

      while (remaining_size > 0)
      {
        size_t const current_chunk_size = (remaining_size >= (chunk_size * 2) ? chunk_size : remaining_size);
        remaining_size -= current_chunk_size;

        std::string s(current_chunk_size, 0);
        ifs.read(&s[0], s.size());

        // encode in base64
        std::string s_b64(bb::encoded_size(s.size()), 0);
        bb::encode(s_b64.data(), s.data(), s.size());

        horcruxes.push_back(std::move(s_b64));
      }

      if (horcruxes.size() != n_chunks)
      {
        std::cerr << "The number of generated horcrux is different from the number expected" << std::endl;
        exit(EXIT_FAILURE);
      }

      ifs.close();
      return horcruxes;
    }

    void generate_file_from_horcruxes(fs::path file_path, std::vector<std::string> const &horcruxes)
    {
      // Check if output file already exists
      if (fs::exists(file_path))
      {
        std::cerr << "Already existing output file " << file_path << std::endl;
        exit(EXIT_FAILURE);
      }

      std::ofstream ofs(file_path, std::ios::binary | std::ios_base::app);

      for (auto const &h_b64 : horcruxes)
      {
        // decode from base64
        std::string s(bb::decoded_size(h_b64.size()), 0);
        bb::decode(s.data(), h_b64.data(), h_b64.size());
        ofs.write(&s[0], s.size());
      }

      ofs.close();
    }

    void save_horcruxes_to_disk(std::vector<std::string> const &horcruxes, std::string uuid, fs::path dir_path)
    {
      // Check if path is a directory
      if (!fs::is_directory(dir_path))
      {
        std::cerr << "Path \"" << dir_path << "\" is not a directory" << std::endl;
        exit(EXIT_FAILURE);
      }

      dir_path /= uuid;

      // Check if uuid directory exists
      if (fs::exists(dir_path))
      {
        std::cerr << "Path \"" << dir_path << "\" already exists" << std::endl;
        exit(EXIT_FAILURE);
      }

      fs::create_directory(dir_path);

      int h_count = 0;
      for (auto const &h : horcruxes)
      {
        std::ofstream ofs(dir_path / generate_horcrux_name(uuid, h_count), std::ios::binary);
        ofs.write(h.data(), h.size());
        ofs.close();
        ++h_count;
      }
    }

    std::vector<std::string> load_horcruxes_from_disk(std::string uuid, fs::path dir_path)
    {
      // Check if path is a directory
      if (!fs::is_directory(dir_path))
      {
        std::cerr << "Path \"" << dir_path << "\" is not a directory" << std::endl;
        exit(EXIT_FAILURE);
      }

      dir_path /= uuid;

      // Check if uuid directory exists
      if (!fs::exists(dir_path))
      {
        std::cerr << "Path \"" << dir_path << "\" doesn't exist" << std::endl;
        exit(EXIT_FAILURE);
      }

      std::vector<std::string> horcruxes;

      int h_count = 0;
      while (fs::exists(dir_path / generate_horcrux_name(uuid, h_count)))
      {
        fs::path current_file{dir_path / generate_horcrux_name(uuid, h_count)};
        std::ifstream ifs(current_file, std::ios::binary);

        std::string s(fs::file_size(current_file), 0);
        ifs.read(s.data(), s.size());

        horcruxes.push_back(std::move(s));

        ifs.close();
        ++h_count;
      }

      return horcruxes;
    }

  }

}