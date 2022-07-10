#ifndef BOTKROK__SOCKETSESSION_H_
#define BOTKROK__SOCKETSESSION_H_
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using message_type = boost::array<char, 30000>;

class SocketSession
{
 public:
    SocketSession(const char* ip, int port);
    void Read(message_type& buf);
    void Write(const std::string& buf);

 private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};
#endif//BOTKROK__SOCKETSESSION_H_
