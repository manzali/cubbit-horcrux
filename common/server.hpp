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

                                                          // check if there is at leat a field
                                                          size_t pos = data.find(hd::field_delimiter);
                                                          if (pos == std::string::npos)
                                                          {
                                                              std::cerr << "error: cannot handle the following request:\n"
                                                                        << data << std::endl;
                                                              return;
                                                          }

                                                          hr::request_type r = hr::get_request_type_from_string(data.substr(0, pos));

                                                          // Skip first delimiter
                                                          pos += hd::field_delimiter.size();

                                                          hd::status_code response_code;

                                                          switch (r)
                                                          {
                                                          case hr::request_type::SAVE:
                                                              response_code = handle_save_request(data, pos);
                                                              break;

                                                          case hr::request_type::LOAD:
                                                              response_code = handle_load_request(data, pos);
                                                              break;

                                                          default:
                                                              response_code = hd::status_code::UNKNOWN_REQUEST;
                                                          }

                                                          // send response
                                                          boost::asio::write(m_socket, boost::asio::buffer(std::to_string(response_code) + hd::message_delimiter));

                                                          // wait for an other request
                                                          wait_for_request();
                                                      }
                                                      else
                                                      {
                                                          std::cerr << "async_read_until error code: " << ec << std::endl;
                                                      }
                                                  }
                                                  catch (const std::exception &e)
                                                  {
                                                      std::cerr << e.what() << '\n';
                                                  }
                                              });
            }

            hd::status_code handle_save_request(std::string &data, size_t cur_pos)
            {
                try
                {
                    // get uuid
                    size_t next_pos = data.find(hd::field_delimiter, cur_pos);
                    if (next_pos == std::string::npos)
                    {
                        throw std::runtime_error("malformed save request");
                    }
                    std::string const uuid = data.substr(cur_pos, (next_pos - cur_pos));

                    std::cout << "uuid: " << uuid << std::endl;

                    // update cur_pos
                    cur_pos = (next_pos + hd::field_delimiter.size());

                    // get index
                    next_pos = data.find(hd::field_delimiter, cur_pos);
                    if (next_pos == std::string::npos)
                    {
                        throw std::runtime_error("malformed save request");
                    }

                    unsigned int index = std::stoi(data.substr(cur_pos, (next_pos - cur_pos)));

                    std::cout << "index: " << index << std::endl;

                    // update cur_pos
                    cur_pos = (next_pos + hd::field_delimiter.size());

                    if (hm::save_horcrux_to_disk(data.erase(0, cur_pos), uuid, index, m_dir))
                    {
                        std::cout << "save request handled successfully" << std::endl;
                    }
                    else
                    {
                        throw std::runtime_error("save request handled with errors");
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    return hd::status_code::SAVE_REQUEST_FAILED;
                }

                return hd::status_code::SAVE_REQUEST_OK;
            }

            hd::status_code handle_load_request(std::string &data, size_t pos)
            {
                // get uuid
                size_t next_pos = data.find(hd::field_delimiter, cur_pos);
                if (next_pos == std::string::npos)
                {
                    throw std::runtime_error("malformed save request");
                }
                std::string const uuid = data.substr(cur_pos, (next_pos - cur_pos));

                std::cout << "uuid: " << uuid << std::endl;

                // std::cout << "handle_load_request:\n"
                //           << hr::serialize_request(obj) << std::endl;

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
