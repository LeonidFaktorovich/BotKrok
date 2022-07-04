#include "SocketSession.h"

using message_type = boost::array<char, 4096>;

SocketSession::SocketSession(const char *ip, int port) : socket_(io_context_) {
    using namespace boost::asio;
    using ip::tcp;
    socket_.connect(tcp::endpoint(ip::address::from_string(ip), port));


}

void SocketSession::Read(message_type &buf) {
    boost::system::error_code error;
    socket_.read_some(boost::asio::buffer(buf), error);
    if (error && error != boost::asio::error::eof) {
        std::cerr << "receive failed: " << error.message() << std::endl;
    }
}
void SocketSession::Write(const std::string &buf) {
    boost::system::error_code error;
    boost::asio::write(socket_, boost::asio::buffer(buf), error);
    if (error && error != boost::asio::error::eof) {
        std::cerr << "receive failed: " << error.message() << std::endl;
    }
}



