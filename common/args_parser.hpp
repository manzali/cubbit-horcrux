#pragma once

#include <filesystem>
#include <string>
#include <iostream>

#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

namespace horcrux
{

    struct client_args
    {
        std::string cmd;
        unsigned int n_chunks;
        fs::path file_path;
    };

    client_args parse_client_args(int argc, char *argv[])
    {
        po::options_description desc("Invocation : <program> <command> -n <chunks> <input_file>");
        client_args args;

        po::positional_options_description p;
        p.add("command", 1);
        p.add("input", 1);

        desc.add_options()("help,h", "print help messages.")("command,c", po::value<std::string>(&args.cmd)->required(), "command [save|load].")("num,n", po::value<unsigned int>(&args.n_chunks)->required(), "number of chunks in which the file will be splitted.")("input,i", po::value<fs::path>(&args.file_path)->required(), "path to the file that will be sent.");

        try
        {

            po::variables_map vm;
            po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

            if (vm.count("help"))
            {
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
            size_t const file_size = fs::file_size(args.file_path);
            if (args.n_chunks > file_size)
            {
                std::cerr << "Number of chunks (" << args.n_chunks << ") cannot be greater than file size in bytes (" << file_size << ")" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        catch (po::error const &e)
        {
            std::cerr << e.what() << '\n'
                      << desc << std::endl;
            exit(EXIT_FAILURE);
        }

        return args;
    }
}