#include "MsgProcess.h"
#include <boost/array.hpp>
#include <vector>
#include <iostream>

map_size::map_size(size_t width, size_t height) {
        map_height = height;
        map_width = width;
}

void game_parameters::SetParameters(const std::string &match_id,
                                    size_t num_rounds,
                                    const map_size &map,
                                    size_t num_bots,
                                    size_t my_id,
                                    size_t view_radius,
                                    size_t mining_radius,
                                    size_t attack_radius,
                                    size_t move_time_limit) {
    this->match_id = match_id;
    this->num_rounds = num_rounds;
    this->map = map;
    this->num_bots = num_bots;
    this->my_id = my_id;
    this->view_radius = view_radius;
    this->mining_radius = mining_radius;
    this->attack_radius = attack_radius;
    this->move_time_limit = move_time_limit;
}

MsgProcess::MsgProcess(std::string bot_name,
                       std::string bot_secret,
                       std::string match_mode) {
    bot_name_ = std::move(bot_name);
    bot_secret_ = std::move(bot_secret);
    match_mode_ = std::move(match_mode);
}

std::string MsgProcess::GetAnswer(const message_type &msg) {
    std::vector<std::string> words;
    boost::split(words, msg.data(), boost::is_any_of("\n "));
    if (words[0] == "hello") {
        protocol_version_ = std::stoi(words[2]);
        return MsgProcess::HelloAnswer();
    } else if (words[0] == "match_started") {
        parameters_.SetParameters(words[2],
                                  std::stoi(words[4]),
                                  map_size(std::stoi(words[8]), std::stoi(words[9])),
                                  std::stoi(words[11]),
                                  std::stoi(words[13]),
                                  std::stoi(words[15]),
                                  std::stoi(words[17]),
                                  std::stoi(words[19]),
                                  std::stoi(words[21]));

        return "";
    } 
    return "";
}

std::string MsgProcess::HelloAnswer() {
    std::string result_string =
        std::string("register") +
        std::string("\n") +
        std::string("bot_name ") +
        bot_name_ +
        std::string("\n") +
        std::string("bot_secret ") +
        bot_secret_ +
        std::string("\n") +
        std::string("mode ") +
        match_mode_ +
        std::string("\n") +
        std::string("end") +
        std::string("\n");
    return result_string;
}

const game_parameters MsgProcess::GetParameters() const {
    return parameters_;
}