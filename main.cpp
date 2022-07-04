#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "MsgProcess.h"
#include "SocketSession.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
int main() {
    /*
    boost::asio::io_context io_context;
//socket creation
    tcp::socket socket(io_context);
//connection
    socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234));
// request/message from client
    boost::system::error_code error;

    // getting response from server
    boost::array<char, 128> buf{};
    for (;;) {
        size_t len = socket.read_some(boost::asio::buffer(buf), error);
        if (len == 0) {
            break;
        }

        if (error && error != boost::asio::error::eof) {
            cout << "receive failed: " << error.message() << endl;
            break;
        } else {
            std::cout.write(buf.data(), len);
            std::cout << std::endl;
        }
        string message_to_send;
        std::cin >> message_to_send;
        if (message_to_send == "") {
            break;
        }
        boost::asio::write( socket, boost::asio::buffer(message_to_send + '\n'), error);
        if (error && error != boost::asio::error::eof) {
            cout << "receive failed: " << error.message() << endl;
            break;
        }
    }
     */
    const char ip[] = "127.0.0.1";
    int port = 1234;
    SocketSession soc(ip, port);
    MsgProcess process("1", "2", "3");
    message_type msg;
    soc.Read(msg);
    std::cout << msg.data() << std::endl;
    auto answer = process.GetAnswer(msg);
    std::cout << answer << std::endl;
    soc.Write(answer);

    return 0;
}
