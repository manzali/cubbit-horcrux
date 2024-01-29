#pragma once

#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>

#include "endpoint.hpp"
#include "dataformat.hpp"
#include "request.hpp"
#include "management.hpp"

namespace horcrux
{

    namespace server
    {

        namespace hr = horcrux::request;
        namespace hm = horcrux::management;
        namespace hd = horcrux::dataformat;

        class session : public std::enable_shared_from_this<session>
        {

        public:
            session(boost::asio::ip::tcp::tcp::socket socket, std::filesystem::path dir) : m_socket(std::move(socket)), m_dir(dir) {}

            void run()
            {
                wait_for_request();
            }

        private:
            void wait_for_request()
            {
                auto self(shared_from_this());
                boost::asio::async_read_until(m_socket, m_buffer, horcrux::dataformat::message_delimiter,
                                              [this, self](boost::system::error_code ec, std::size_t length)
                                              {
                                                  try
                                                  {
                                                      if (!ec)
                                                      {
                                                          std::string data{
                                                              std::istreambuf_iterator<char>(&m_buffer),
                                                              std::istreambuf_iterator<char>()};

                                                          hr::request_obj obj;
                                                          if (!hr::parse_request(data, obj))
                                                          {
                                                              throw std::runtime_error("malformed request");
                                                          }

                                                          hd::status_code response_code;

                                                          switch (hr::get_request_type_from_object(obj))
                                                          {
                                                          case hr::request_type::SAVE:
                                                              response_code = handle_save_request(obj);
                                                              break;

                                                          case hr::request_type::LOAD:
                                                              response_code = handle_load_request(obj);
                                                              break;

                                                          default:
                                                              response_code = hd::status_code::UNKNOWN_REQUEST;
                                                          }

                                                          // send response
                                                          boost::asio::write(m_socket, boost::asio::buffer(hr::generate_save_reply(response_code) + hd::message_delimiter));

                                                          // wait for an other request
                                                          wait_for_request();
                                                      }
                                                      else
                                                      {
                                                          std::cout << "closing session\n"
                                                                    << std::endl;
                                                      }
                                                  }
                                                  catch (const std::exception &e)
                                                  {
                                                      std::cerr << e.what() << '\n';
                                                  }
                                              });
            }

            hd::status_code handle_save_request(hr::request_obj const &obj)
            {
                try
                {
                    std::string uuid;
                    unsigned int index, total;
                    std::string horcrux;

                    if (!hr::parse_save_request(obj, uuid, index, total, horcrux))
                    {
                        throw std::runtime_error("malformed save request");
                    }

                    if (!hm::save_horcrux_to_disk(horcrux, uuid, index, m_dir))
                    {
                        std::cerr << "error saving horcrux to disk" << std::endl;
                        return hd::status_code::ERROR_SAVING_HORCRUX;
                    }

                    std::cout << "save request handled successfully" << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    return hd::status_code::SAVE_REQUEST_FAILED;
                }

                return hd::status_code::SAVE_REQUEST_OK;
            }

            hd::status_code handle_load_request(hr::request_obj const &obj)
            {
                try
                {
                    std::string uuid;

                    if (!hr::parse_load_request(obj, uuid))
                    {
                        throw std::runtime_error("malformed load request");
                    }

                    std::vector<std::string> horcruxes;

                    if (!hm::load_horcruxes_from_disk(uuid, m_dir, horcruxes))
                    {
                        std::cerr << "error loading horcruxes from disk" << std::endl;
                        hd::status_code status_code = hd::status_code::ERROR_LOADING_HORCRUXES;
                        boost::asio::write(m_socket, boost::asio::buffer(hr::generate_load_reply(status_code) + horcrux::dataformat::message_delimiter));
                        return status_code;
                    }

                    unsigned int total = horcruxes.size();
                    int status_code = hd::status_code::HORCRUXES_FOUND;
                    for (unsigned int index = 0; index < total; ++index)
                    {
                        boost::asio::write(m_socket, boost::asio::buffer(hr::generate_load_reply(status_code, index, total, horcruxes[index]) + horcrux::dataformat::message_delimiter));
                    }

                    std::cout << "load request handled successfully" << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    return hd::status_code::LOAD_REQUEST_FAILED;
                }

                return hd::status_code::LOAD_REQUEST_OK;
            }

        private:
            boost::asio::ip::tcp::tcp::socket m_socket;
            boost::asio::streambuf m_buffer;
            std::filesystem::path m_dir;
        };

        class server
        {

        public:
            server(horcrux::endpoint::endpoint const &ep, std::filesystem::path dir)
                : m_io_context(), m_acceptor(m_io_context, ep), m_dir(dir)
            {
                do_accept();
            }

            void start()
            {
                m_io_context.run();
            }

        private:
            void do_accept()
            {
                m_acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::tcp::socket socket)
                                        {
            if (!ec) {
                std::cout << "creating session on: " 
                    << socket.remote_endpoint().address().to_string() 
                    << ":" << socket.remote_endpoint().port() << '\n';

                std::make_shared<session>(std::move(socket), m_dir)->run();
            } else {
                std::cout << "error: " << ec.message() << std::endl;
            }
            do_accept(); });
            }

        private:
            boost::asio::io_context m_io_context;
            boost::asio::ip::tcp::tcp::acceptor m_acceptor;
            std::filesystem::path m_dir;
        };

    }

}
