#include "Bot.h"

Bot::Bot(std::string bot_name, std::string bot_secret, std::string match_mode) : bot_name_(bot_name), bot_secret_(bot_secret), match_mode_(match_mode) {
}

void Bot::StartSession(const char *ip, int port) {
    socket_session_ = new SocketSession(ip, port);
    message_type hello_msg;
    socket_session_->Read(hello_msg);
    std::string answer = MsgProcess::HelloAnswer(bot_name_, bot_secret_, match_mode_);
    socket_session_->Write(answer);
    message_type data_msg;
    socket_session_->Read(data_msg);
    game_parameters gameParameters;
    MsgProcess::GetParameters(data_msg, gameParameters);
    algorithm = new Algorithm(gameParameters);
    // тест ответа
    for (size_t i = 0; i < gameParameters.num_rounds; ++i) {
        message_type msg;
        socket_session_->Read(msg);
        answer = "move\noffset -1 0\nend\n";
        socket_session_->Write(answer);
    }
    message_type finish_msg;
    socket_session_->Read(finish_msg);
}

Bot::~Bot() {
    delete socket_session_;
    delete algorithm;
}
