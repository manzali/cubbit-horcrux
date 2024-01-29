#pragma once

#include <boost/asio.hpp>

#include "horcrux/management.hpp"
#include "horcrux/request.hpp"
#include "horcrux/dataformat.hpp"

namespace horcrux
{
    namespace client
    {

        class client
        {
        public:
            client() : m_io_context(), m_socket(m_io_context), m_resolver(m_io_context)
            {
            }

            void connect(std::string const &host, std::string const &port)
            {
                boost::asio::connect(m_socket, m_resolver.resolve(host, port));
            }

            void disconnect()
            {
                boost::system::error_code ec;
                m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                m_socket.close();
            }

            int send_save_request(std::filesystem::path const &file_path, unsigned int n_chunks)
            {
                try
                {
                    std::vector<std::string> horcruxes;
                    if (!horcrux::management::generate_horcruxes_from_file(file_path, n_chunks, horcruxes))
                    {
                        std::cerr << "error generate_horcruxes_from_file" << std::endl;
                        return horcrux::dataformat::status_code::ERROR_GENERATING_HORCRUXES;
                    }

                    std::string uuid = horcrux::management::generate_uuid();

                    for (unsigned int index = 0; index < horcruxes.size(); ++index)
                    {
                        std::string const req = horcrux::request::generate_save_request(uuid, index, horcruxes.size(), horcruxes[index]);

                        boost::system::error_code ec;
                        boost::asio::write(m_socket, boost::asio::buffer(req + horcrux::dataformat::message_delimiter), ec);
                        if (ec)
                        {
                            std::cerr << "write error code: " << ec << std::endl;
                            return horcrux::dataformat::status_code::SAVE_REQUEST_FAILED;
                        }

                        boost::asio::streambuf buffer;
                        boost::asio::read_until(m_socket, buffer, horcrux::dataformat::message_delimiter, ec);

                        if (ec)
                        {
                            std::cerr << "read_until error code: " << ec << std::endl;
                            return horcrux::dataformat::status_code::SAVE_REQUEST_FAILED;
                        }

                        hr::request_obj obj;
                        if (!hr::parse_request({std::istreambuf_iterator<char>(&buffer),
                                                std::istreambuf_iterator<char>()},
                                               obj))
                        {
                            throw std::runtime_error("malformed message");
                        }

                        int status_code = -1;
                        if (!hr::parse_save_reply(obj, status_code))
                        {
                            throw std::runtime_error("malformed save reply message");
                        }

                        if (status_code != horcrux::dataformat::status_code::SAVE_REQUEST_OK)
                        {
                            std::cerr << "server returned status code " << status_code << std::endl;
                            return status_code;
                        }
                    }

                    return horcrux::dataformat::status_code::SAVE_REQUEST_OK;
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    return horcrux::dataformat::status_code::SAVE_REQUEST_FAILED;
                }
            }

            int send_load_request(std::string const &uuid, std::filesystem::path const &file_path)
            {
                try
                {
                    boost::system::error_code ec;
                    boost::asio::write(m_socket, boost::asio::buffer(horcrux::request::generate_load_request(uuid) + horcrux::dataformat::message_delimiter), ec);

                    if (ec)
                    {
                        std::cerr << "write error code: " << ec << std::endl;
                        return horcrux::dataformat::status_code::LOAD_REQUEST_FAILED;
                    }

                    std::map<unsigned int, std::string> horcruxes;

                    unsigned int total = 1;
                    do
                    {
                        boost::asio::streambuf buffer;
                        boost::asio::read_until(m_socket, buffer, horcrux::dataformat::message_delimiter, ec);

                        if (ec)
                        {
                            std::cerr << "read_until error code: " << ec << std::endl;
                            return horcrux::dataformat::status_code::LOAD_REQUEST_FAILED;
                        }

                        hr::request_obj obj;
                        if (!hr::parse_request({std::istreambuf_iterator<char>(&buffer),
                                                std::istreambuf_iterator<char>()},
                                               obj))
                        {
                            throw std::runtime_error("malformed message");
                        }

                        int status_code;
                        unsigned int index;
                        std::string horcrux;
                        if (!hr::parse_load_reply(obj, status_code, index, total, horcrux))
                        {
                            throw std::runtime_error("malformed load reply message");
                        }

                        horcruxes.emplace(index, std::move(horcrux));

                    } while (horcruxes.size() < total);

                    if (!horcrux::management::generate_file_from_horcruxes(file_path, horcruxes))
                    {
                        std::cerr << "error generate_file_from_horcruxes" << std::endl;
                        return horcrux::dataformat::status_code::ERROR_SAVING_FILE;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    return horcrux::dataformat::status_code::LOAD_REQUEST_FAILED;
                }

                return horcrux::dataformat::status_code::LOAD_REQUEST_OK;
            }

        private:
            boost::asio::io_context m_io_context;
            boost::asio::ip::tcp::socket m_socket;
            boost::asio::ip::tcp::resolver m_resolver;
        };

    }
}