#include "Bot.h"
#include <chrono>

Bot::Bot(std::string bot_name, std::string bot_secret, MatchMode match_mode) : bot_name_(bot_name),
                                                                                 bot_secret_(bot_secret),
                                                                                 match_mode_(match_mode) {
}

void Bot::StartSession(const char *ip, int port) {
    socket_session_ = new SocketSession(ip, port);
    message_type hello_msg;
    socket_session_->Read(hello_msg);
    std::string answer;
    if (match_mode_ == MatchMode::FRIENDLY) {
        answer = MsgProcess::HelloAnswer(bot_name_, bot_secret_, "FRIENDLY");

    } else if (match_mode_ == MatchMode::DEATHMATCH) {
        answer = MsgProcess::HelloAnswer(bot_name_, bot_secret_, "DEATHMATCH");

    }
    socket_session_->Write(answer);
    message_type data_msg;
    socket_session_->Read(data_msg);
    game_parameters gameParameters;
    bool was_game_data = MsgProcess::GetParameters(data_msg, gameParameters, match_mode_);
    size_t my_id = gameParameters.my_id;
    algorithm = new Algorithm(gameParameters);
    // тест ответа

    for (size_t i = 0; i < gameParameters.num_rounds; ++i) {
        message_type msg;
        std::cout << "Step " << i << std::endl;
        //auto start_read = std::chrono::steady_clock::now();

        if (was_game_data) {
            msg = data_msg;
            was_game_data = false;
        } else {
            socket_session_->Read(msg);
//            std::vector<std::string> lines;
//            while (lines.empty() || lines[0] != "update") {
//                lines.clear();
//                socket_session_->Read(msg);
//                boost::split(lines, msg.data(), boost::is_any_of("\n"));
//            }
        }

        //std::cout << msg.data() << std::endl;

        //auto finish_read = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish_read - start_read).count() << " получение данных: ";

        //auto start_data = std::chrono::steady_clock::now();

        std::pair<int, int> no_info = {-1, -1};

        auto my_pos = MsgProcess::GetData(msg, my_id, *algorithm);
        if (my_pos == no_info) {
            --i;
            continue;
        }
        //auto finish_data = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish_data - start_data).count() << " запись: ";
        //auto start_write = std::chrono::steady_clock::now();
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
        history_of_step_.emplace_back(x_shift, y_shift);
        answer = std::string("move\noffset ") + std::to_string(x_shift) + std::string(" ") + std::to_string(y_shift) + std::string("\nend\n");
        socket_session_->Write(answer);
        //auto finish_write = std::chrono::steady_clock::now();


        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish_write - start_write).count() << std::endl;
    }
    message_type finish_msg;
    socket_session_->Read(finish_msg);
}

Bot::~Bot() {
    delete socket_session_;
    delete algorithm;
}
