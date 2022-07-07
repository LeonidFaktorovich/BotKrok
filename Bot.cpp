#include "Bot.h"
Bot::Bot(std::string bot_name, std::string bot_secret, std::string match_mode) : bot_name_(bot_name),
                                                                                 bot_secret_(bot_secret),
                                                                                 match_mode_(match_mode) {
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
    size_t my_id = gameParameters.my_id;
    algorithm = new Algorithm(gameParameters);
    // тест ответа

    for (size_t i = 0; i < gameParameters.num_rounds; ++i) {
        message_type msg;
        std::cout << "Step " << i << ' ' << std::endl;
        socket_session_->Read(msg);

        auto my_pos = MsgProcess::GetData(msg, my_id, *algorithm);
        auto [new_x, new_y] = algorithm->GetNextStep(my_pos, history_of_step_.back());
        int x_shift = static_cast<int>(new_x) - static_cast<int>(my_pos.first);
        if (x_shift == static_cast<int>(gameParameters.map.map_width) - 1) {
            x_shift = -1;
        } else if (x_shift == -static_cast<int>(gameParameters.map.map_width) + 1) {
            x_shift = 1;
        }
        int y_shift = (static_cast<int>(new_y) - static_cast<int>(my_pos.second));
        if (y_shift == static_cast<int>(gameParameters.map.map_height) - 1) {
            y_shift = -1;
        } else if (y_shift == -static_cast<int>(gameParameters.map.map_height) + 1) {
            y_shift = 1;
        }
        history_of_step_.emplace_back(static_cast<int>(x_shift), static_cast<int>(y_shift));
        answer = std::string("move\noffset ") + std::to_string(x_shift) + std::string(" ") + std::to_string(y_shift) + std::string("\nend\n");
        socket_session_->Write(answer);
    }
    message_type finish_msg;
    socket_session_->Read(finish_msg);
}

Bot::~Bot() {
    delete socket_session_;
    delete algorithm;
}
