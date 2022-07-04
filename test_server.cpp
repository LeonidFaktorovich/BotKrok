#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
string read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "end");
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(message) );
}
int main() {
    boost::asio::io_context io_context;
//listen for new connection
    tcp::acceptor acceptor_(io_context, tcp::endpoint(tcp::v4(), 1234));
//socket creation
    tcp::socket socket_(io_context);
//waiting for connection
    acceptor_.accept(socket_);
//read operation
/*
    for (;;) {
//write operation
        string message_to_send;
        std::cin >> message_to_send;
        if (message_to_send == "") {
            break;
        }

        send_(socket_, message_to_send);
        string message = read_(socket_);
        cout << message.substr(0, message.size() - 1) << endl;
    }
    */
    string message_to_send = "hello\nprotocol_version 1\nend";
    send_(socket_, message_to_send);
    string message = read_(socket_);
    cout << message << endl;
    return 0;
}
