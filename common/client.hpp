#pragma once

#include <boost/asio.hpp>

#include "management.hpp"
#include "request.hpp"
#include "dataformat.hpp"

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
                    std::vector<std::string> horcruxes{horcrux::management::generate_horcruxes_from_file(file_path, n_chunks)};
                    std::string uuid = horcrux::management::generate_uuid();

                    for (unsigned int index = 0; index < horcruxes.size(); ++index)
                    {
                        std::string const req = horcrux::request::generate_save_request(uuid, index, horcruxes[index]);
                        boost::asio::write(m_socket, boost::asio::buffer(req));

                        boost::system::error_code ec;
                        boost::asio::streambuf buffer;
                        boost::asio::read_until(m_socket, buffer, horcrux::dataformat::message_delimiter, ec);

                        if (ec)
                        {
                            std::cerr << "async_read error code: " << ec << std::endl;
                            return horcrux::dataformat::status_code::SAVE_REQUEST_FAILED;
                        }

                        std::istream str(&buffer);
                        std::string s;
                        std::getline(str, s);

                        int ret = std::stoi(s);
                        if (ret != horcrux::dataformat::status_code::SAVE_REQUEST_OK)
                        {
                            std::cerr << "server returned status code " << ret << std::endl;
                            return ret;
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

            int send_load_request(std::string const &file_id, std::filesystem::path const &file_path)
            {
                try
                {
                    std::vector<std::string> horcruxes;
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