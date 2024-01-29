#pragma once

#include <filesystem>
#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "dataformat.hpp"
#include "request.hpp"

namespace fs = std::filesystem;

namespace hd = horcrux::dataformat;
namespace hr = horcrux::request;

namespace horcrux
{
    namespace args
    {

        struct client_args
        {
            hr::request_type req;
            unsigned int n_chunks;
            fs::path file_path;
            std::string file_id;
        };

        std::string const help_description = "Wrong arguments [TODO: improve this]";

        client_args parse_client_args(int argc, char *argv[])
        {
            client_args args;

            if (argc < 2)
            {
                std::cerr << help_description << std::endl;
                exit(EXIT_FAILURE);
            }

            switch (hr::get_request_type_from_string(argv[1]))
            {
            case hr::request_type::SAVE:
            {
                args.req = hr::request_type::SAVE;
                if (argc < 5 || !boost::iequals(argv[2], "-n"))
                {
                    std::cerr << help_description << std::endl;
                    exit(EXIT_FAILURE);
                }

                // get horcrux count
                try
                {
                    args.n_chunks = std::stoi(argv[3]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cerr << help_description << std::endl;
                    exit(EXIT_FAILURE);
                }

                args.file_path = argv[4];

                if (!fs::exists(args.file_path))
                {
                    std::cerr << "Missing input file " << args.file_path << std::endl;
                    exit(EXIT_FAILURE);
                }

                // check if the number of chunk is greater then the file size
                size_t const file_size = fs::file_size(args.file_path);
                if (args.n_chunks > file_size)
                {
                    std::cerr << "Number of chunks (" << args.n_chunks << ") cannot be greater than file size in bytes (" << file_size << ")" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            break;

            case hr::request_type::LOAD:
            {
                args.req = hr::request_type::LOAD;

                if (argc < 4)
                {
                    std::cerr << help_description << std::endl;
                    exit(EXIT_FAILURE);
                }

                args.file_id = argv[2];   // TODO: check if it is a valid uuid
                args.file_path = argv[3]; // TODO: check if it is a valid path (at list the directory)
            }
            break;

            case hr::request_type::UNKNOWN:
            default:
            {
                std::cerr << help_description << std::endl;
                exit(EXIT_FAILURE);
            }
            }

            return args;
        }
    }
}