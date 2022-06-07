# HTTP_on_TCP
Code for experiment for comparing with QUIC


test scenario

1) wifi -> 5G
2) 5G -> wifi
3) ping pong


![스크린샷, 2022-04-15 00-27-40](https://user-images.githubusercontent.com/62577565/163423296-b66e310b-3196-494f-adc2-adf02b62adf9.png)

![스크린샷, 2022-04-15 00-21-18](https://user-images.githubusercontent.com/62577565/163423299-8fce17bf-9eba-4cfc-b4cf-953d2b046077.png)

![스크린샷, 2022-04-15 00-36-49](https://user-images.githubusercontent.com/62577565/163424679-f552ee67-7e00-4a59-a83c-819cbf8dce3f.png)


#ifndef CLIENT_HTTP_HPP
#define CLIENT_HTTP_HPP

#include <boost/asio.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <map>
#include <random>
#include <mutex>
#include <type_traits>

#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
#ifndef CASE_INSENSITIVE_EQUALS_AND_HASH
#define CASE_INSENSITIVE_EQUALS_AND_HASH
// Based on http://www.boost.org/doc/libs/1_60_0/doc/html/unordered/hash_equality.html
class case_insensitive_equals
{
public:
    bool operator()(const std::string &key1, const std::string &key2) const
    {
        return boost::algorithm::iequals(key1, key2);
    }
};
class case_insensitive_hash
{
public:
    size_t operator()(const std::string &key) const
    {
        std::size_t seed = 0;
        for (auto &c : key)
            boost::hash_combine(seed, std::tolower(c));
        return seed;
    }
};
#endif

namespace SimpleWeb
{
    template <class socket_type>
    class Client;

    template <class socket_type>
    class ClientBase
    {
    public:
        virtual ~ClientBase() {}

        class Response
        {
            friend class ClientBase<socket_type>;
            friend class Client<socket_type>;

        public:
            std::string http_version, status_code;

            std::istream content;

            std::unordered_multimap<std::string, std::string, case_insensitive_hash, case_insensitive_equals> header;

        private:
            boost::asio::streambuf content_buffer;

            Response() : content(&content_buffer) {}
        };

        class Config
        {
            friend class ClientBase<socket_type>;

        private:
            Config() {}

        public:
            /// Set timeout on requests in seconds. Default value: 0 (no timeout).
            size_t timeout = 0;
            /// Set connect timeout in seconds. Default value: 0 (Config::timeout is then used instead).
            size_t timeout_connect = 0;

            /// Set proxy server (server:port)
            std::string proxy_server;
            
            bool reuse_address = true;
        };

        /// Set before calling request
        Config config;

        std::shared_ptr<Response> request(const std::string &request_type, const std::string &path = "/", boost::string_ref content = "",
                                          const std::map<std::string, std::string> &header = std::map<std::string, std::string>())
        {
            auto corrected_path = path;
            if (corrected_path == "")
                corrected_path = "/";
            if (!config.proxy_server.empty() && std::is_same<socket_type, boost::asio::ip::tcp::socket>::value)
                corrected_path = "http://" + host + ':' + std::to_string(port) + corrected_path;

            boost::asio::streambuf write_buffer;
            std::ostream write_stream(&write_buffer);
            write_stream << request_type << " " << corrected_path << " HTTP/1.1\r\n";
            write_stream << "Host: " << host << "\r\n";
            for (auto &h : header)
            {
                write_stream << h.first << ": " << h.second << "\r\n";
            }
            if (content.size() > 0)
                write_stream << "Content-Length: " << content.size() << "\r\n";
            write_stream << "\r\n";

            connect();

            auto timer = get_timeout_timer();
            boost::asio::async_write(*socket, write_buffer,
                                     [this, &content, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                     {
                                         if (timer)
                                             timer->cancel();
                                         if (!ec)
                                         {
                                             if (!content.empty())
                                             {
                                                 auto timer = get_timeout_timer();
                                                 boost::asio::async_write(*socket, boost::asio::buffer(content.data(), content.size()),
                                                                          [this, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                                                          {
                                                                              if (timer)
                                                                                  timer->cancel();
                                                                              if (ec)
                                                                              {
                                                                                  std::lock_guard<std::mutex> lock(socket_mutex);
                                                                                  this->socket = nullptr;
                                                                                  throw boost::system::system_error(ec);
                                                                              }
                                                                          });
                                             }
                                         }
                                         else
                                         {
                                             std::lock_guard<std::mutex> lock(socket_mutex);
                                             socket = nullptr;
                                             throw boost::system::system_error(ec);
                                         }
                                     });
            io_service.reset();
            io_service.run();

            return request_read();
        }

        std::shared_ptr<Response> request(const std::string &request_type, const std::string &path, std::iostream &content,
                                          const std::map<std::string, std::string> &header = std::map<std::string, std::string>())
        {
            auto corrected_path = path;
            if (corrected_path == "")
                corrected_path = "/";
            if (!config.proxy_server.empty() && std::is_same<socket_type, boost::asio::ip::tcp::socket>::value)
                corrected_path = "http://" + host + ':' + std::to_string(port) + corrected_path;

            content.seekp(0, std::ios::end);
            auto content_length = content.tellp();
            content.seekp(0, std::ios::beg);

            boost::asio::streambuf write_buffer;
            std::ostream write_stream(&write_buffer);
            write_stream << request_type << " " << corrected_path << " HTTP/1.1\r\n";
            write_stream << "Host: " << host << "\r\n";
            for (auto &h : header)
            {
                write_stream << h.first << ": " << h.second << "\r\n";
            }
            if (content_length > 0)
                write_stream << "Content-Length: " << content_length << "\r\n";
            write_stream << "\r\n";
            if (content_length > 0)
                write_stream << content.rdbuf();

            connect();

            auto timer = get_timeout_timer();
            boost::asio::async_write(*socket, write_buffer,
                                     [this, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                     {
                                         if (timer)
                                             timer->cancel();
                                         if (ec)
                                         {
                                             std::lock_guard<std::mutex> lock(socket_mutex);
                                             socket = nullptr;
                                             throw boost::system::system_error(ec);
                                         }
                                     });
            io_service.reset();
            io_service.run();

            return request_read();
        }

        void close()
        {
            std::lock_guard<std::mutex> lock(socket_mutex);
            if (socket)
            {
                boost::system::error_code ec;
                socket->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                socket->lowest_layer().close();

                // std::cout << "end_time : " << end_time << "초" << std::endl;
                // std::cout << "start_time : " << start_time << "초" << std::endl;
            }
        }

    protected:
        boost::asio::io_service io_service;
        std::shared_ptr<boost::asio::io_service> io_service2;
        boost::asio::ip::tcp::resolver resolver;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
        std::unique_ptr<socket_type> socket;
        std::mutex socket_mutex;

        std::string host;
        unsigned short port;
        std::string client;
        

        ClientBase(const std::string &host_port, unsigned short host_default_port, const std::string &client_ip) : resolver(io_service)
        {
            auto parsed_host_port = parse_host_port(host_port, host_default_port);
            host = parsed_host_port.first;
            port = parsed_host_port.second;

            client = client_ip;
        }

        std::pair<std::string, unsigned short> parse_host_port(const std::string &host_port, unsigned short host_default_port)
        {
            std::pair<std::string, unsigned short> parsed_host_port;
            size_t host_end = host_port.find(':');
            if (host_end == std::string::npos)
            {
                parsed_host_port.first = host_port;
                parsed_host_port.second = host_default_port;
            }
            else
            {
                parsed_host_port.first = host_port.substr(0, host_end);
                parsed_host_port.second = static_cast<unsigned short>(stoul(host_port.substr(host_end + 1)));
            }
            return parsed_host_port;
        }

        virtual void connect() = 0;

        std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(size_t timeout = 0)
        {
            if (timeout == 0)
                timeout = config.timeout;
            if (timeout == 0)
                return nullptr;

            auto timer = std::make_shared<boost::asio::deadline_timer>(io_service);
            timer->expires_from_now(boost::posix_time::seconds(timeout));
            timer->async_wait([this](const boost::system::error_code &ec)
                              {
                if(!ec) {
                    close();
                } });
            return timer;
        }

        void parse_response_header(const std::shared_ptr<Response> &response) const
        {
            std::string line;
            getline(response->content, line);
            size_t version_end = line.find(' ');
            if (version_end != std::string::npos)
            {
                if (5 < line.size())
                    response->http_version = line.substr(5, version_end - 5);
                if ((version_end + 1) < line.size())
                    response->status_code = line.substr(version_end + 1, line.size() - (version_end + 1) - 1);

                getline(response->content, line);
                size_t param_end;
                while ((param_end = line.find(':')) != std::string::npos)
                {
                    size_t value_start = param_end + 1;
                    if ((value_start) < line.size())
                    {
                        if (line[value_start] == ' ')
                            value_start++;
                        if (value_start < line.size())
                            response->header.insert(std::make_pair(line.substr(0, param_end), line.substr(value_start, line.size() - value_start - 1)));
                    }

                    getline(response->content, line);
                }
            }
        }

        std::shared_ptr<Response> request_read()
        {
            std::shared_ptr<Response> response(new Response());

            boost::asio::streambuf chunked_streambuf;

            auto timer = get_timeout_timer();
            boost::asio::async_read_until(*socket, response->content_buffer, "\r\n\r\n",
                                          [this, &response, &chunked_streambuf, timer](const boost::system::error_code &ec, size_t bytes_transferred)
                                          {
                                              if (timer)
                                                  timer->cancel();
                                              if (!ec)
                                              {
                                                  size_t num_additional_bytes = response->content_buffer.size() - bytes_transferred;

                                                  parse_response_header(response);

                                                  auto header_it = response->header.find("Content-Length");
                                                  if (header_it != response->header.end())
                                                  {
                                                      auto content_length = stoull(header_it->second);
                                                      if (content_length > num_additional_bytes)
                                                      {
                                                          auto timer = get_timeout_timer();
                                                          boost::asio::async_read(*socket, response->content_buffer,
                                                                                  boost::asio::transfer_exactly(content_length - num_additional_bytes),
                                                                                  [this, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                                                                  {
                                                                                      if (timer)
                                                                                          timer->cancel();
                                                                                      if (ec)
                                                                                      {
                                                                                          std::lock_guard<std::mutex> lock(socket_mutex);
                                                                                          this->socket = nullptr;
                                                                                          throw boost::system::system_error(ec);
                                                                                      }
                                                                                  });
                                                      }
                                                  }
                                                  else if ((header_it = response->header.find("Transfer-Encoding")) != response->header.end() && header_it->second == "chunked")
                                                  {
                                                      request_read_chunked(response, chunked_streambuf);
                                                  }
                                                  else if (response->http_version < "1.1" || ((header_it = response->header.find("Connection")) != response->header.end() && header_it->second == "close"))
                                                  {
                                                      auto timer = get_timeout_timer();
                                                      boost::asio::async_read(*socket, response->content_buffer,
                                                                              [this, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                                                              {
                                                                                  if (timer)
                                                                                      timer->cancel();
                                                                                  if (ec)
                                                                                  {
                                                                                      std::lock_guard<std::mutex> lock(socket_mutex);
                                                                                      this->socket = nullptr;
                                                                                      if (ec != boost::asio::error::eof)
                                                                                          throw boost::system::system_error(ec);
                                                                                  }
                                                                              });
                                                  }
                                              }
                                              else
                                              {
                                                  std::lock_guard<std::mutex> lock(socket_mutex);
                                                  socket = nullptr;
                                                  throw boost::system::system_error(ec);
                                              }
                                          });
            io_service.reset();
            io_service.run();

            return response;
        }

        void request_read_chunked(const std::shared_ptr<Response> &response, boost::asio::streambuf &streambuf)
        {
            auto timer = get_timeout_timer();
            boost::asio::async_read_until(*socket, response->content_buffer, "\r\n",
                                          [this, &response, &streambuf, timer](const boost::system::error_code &ec, size_t bytes_transferred)
                                          {
                                              if (timer)
                                                  timer->cancel();
                                              if (!ec)
                                              {
                                                  std::string line;
                                                  getline(response->content, line);
                                                  bytes_transferred -= line.size() + 1;
                                                  line.pop_back();
                                                  std::streamsize length = stol(line, 0, 16);

                                                  auto num_additional_bytes = static_cast<std::streamsize>(response->content_buffer.size() - bytes_transferred);

                                                  auto post_process = [this, &response, &streambuf, length]
                                                  {
                                                      std::ostream stream(&streambuf);
                                                      if (length > 0)
                                                      {
                                                          std::vector<char> buffer(static_cast<size_t>(length));
                                                          response->content.read(&buffer[0], length);
                                                          stream.write(&buffer[0], length);
                                                      }

                                                      // Remove "\r\n"
                                                      response->content.get();
                                                      response->content.get();

                                                      if (length > 0)
                                                          request_read_chunked(response, streambuf);
                                                      else
                                                      {
                                                          std::ostream response_stream(&response->content_buffer);
                                                          response_stream << stream.rdbuf();
                                                      }
                                                  };

                                                  if ((2 + length) > num_additional_bytes)
                                                  {
                                                      auto timer = get_timeout_timer();
                                                      boost::asio::async_read(*socket, response->content_buffer,
                                                                              boost::asio::transfer_exactly(2 + length - num_additional_bytes),
                                                                              [this, post_process, timer](const boost::system::error_code &ec, size_t /*bytes_transferred*/)
                                                                              {
                                                                                  if (timer)
                                                                                      timer->cancel();
                                                                                  if (!ec)
                                                                                  {
                                                                                      post_process();
                                                                                  }
                                                                                  else
                                                                                  {
                                                                                      std::lock_guard<std::mutex> lock(socket_mutex);
                                                                                      this->socket = nullptr;
                                                                                      throw boost::system::system_error(ec);
                                                                                  }
                                                                              });
                                                  }
                                                  else
                                                      post_process();
                                              }
                                              else
                                              {
                                                  std::lock_guard<std::mutex> lock(socket_mutex);
                                                  socket = nullptr;
                                                  throw boost::system::system_error(ec);
                                              }
                                          });
        }
    };

    template <class socket_type>
    class Client : public ClientBase<socket_type>
    {
    };

    typedef boost::asio::ip::tcp::socket HTTP;

    template <>
    class Client<HTTP> : public ClientBase<HTTP>
    {
    public:
        Client(const std::string &server_port_path, const std::string &client_port_path) : ClientBase<HTTP>::ClientBase(server_port_path, 80, client_port_path) {}
        std::size_t total_delay_start_time;
        std::size_t handover_delay_end_time;

    protected:
     

        void connect()
        {
            auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            total_delay_start_time = millisec_since_epoch;
            handover_delay_end_time = millisec_since_epoch;
            
          
            if (!socket || !socket->is_open())
            {

                std::unique_ptr<boost::asio::ip::tcp::resolver::query> query;
                
                if (config.proxy_server.empty())
                { 
                    query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(new boost::asio::ip::tcp::resolver::query(host, std::to_string(port)));
                }
                else
                {
                   auto proxy_host_port = parse_host_port(config.proxy_server, 8080); // host ip 와 port 를 나눈다.
                    query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(new boost::asio::ip::tcp::resolver::query(proxy_host_port.first, std::to_string(proxy_host_port.second)));
                }
              
               
                        
                        std::cout << "hi" << std::endl;
                        
                        boost::asio::ip::tcp::endpoint endpoint;

                        boost::system::error_code err_c;

                        endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(client), 8081); //Ip 주소 
                        boost::asio::ip::tcp::endpoint endpoint_server = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("155.230.34.228"), 8080);
                        
                        
                        boost::asio::ip::tcp::socket sock(io_service, endpoint.protocol());
                       // sock.open(endpoint.protocol());
                        sock.bind(endpoint, err_c);
                        std::cout << "bind" << std::endl;
                         std::cout << err_c << std::endl;
                        std::cout << endpoint << std::endl;
                        
                        
                        //socket->async_connect();   
                        auto timer=get_timeout_timer(config.timeout_connect);
                        std::cout << "ip" << std::endl;
                        boost::asio::ip::address addr = sock.local_endpoint().address();
                        std::cout << addr<< std::endl;
                        std::cout << sock.local_endpoint()<< std::endl;

                        
                       sock.async_connect(endpoint_server, [timer](const boost::system::error_code &ec){
                            if(timer)
                                timer->cancel();
                            if(!ec) {
                                std::cout << "1" << std::endl;
                                boost::asio::ip::tcp::no_delay option(true);
                                //this->socket->set_option(option);
                                
                                std::cout << "2" << std::endl;
                            }
                            else {
                                
                                std::cout << "3" << std::endl;
                                //std::lock_guard<std::mutex> lock(socket_mutex);
                                //this->socket=nullptr;
                                std::cout << "4" << std::endl;
                                throw boost::system::system_error(ec);
                            }});

                        // boost::asio::async_connect(sock, it, [timer]
                        //         (const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator /*it*/){
                        //     if(timer)
                        //         timer->cancel();
                        //     if(!ec) {
                        //         std::cout << "1" << std::endl;
                        //         boost::asio::ip::tcp::no_delay option(true);
                        //         //this->socket->set_option(option);
                                
                        //         std::cout << "2" << std::endl;
                        //     }
                        //     else {
                                
                        //         std::cout << "3" << std::endl;
                        //         //std::lock_guard<std::mutex> lock(socket_mutex);
                        //         //this->socket=nullptr;
                        //         std::cout << "4" << std::endl;
                        //         throw boost::system::system_error(ec);
                        //     }
                        // });
                   
                //std::cout << "10" << std::endl;
                io_service.reset();
                //std::cout << "11" << std::endl;
                io_service.run();
                
            }
        }
    };
}

#endif /* CLIENT_HTTP_HPP */

