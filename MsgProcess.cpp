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


std::string MsgProcess::HelloAnswer(const std::string &bot_name, const std::string &bot_secret, const std::string &match_mode) {
    std::string result_string =
        std::string("register") +
        std::string("\n") +
        std::string("bot_name ") +
        bot_name +
        std::string("\n") +
        std::string("bot_secret ") +
        bot_secret +
        std::string("\n") +
        std::string("mode ") +
        match_mode +
        std::string("\n") +
        std::string("end") +
        std::string("\n");
    return result_string;
}

void MsgProcess::GetParameters(const message_type &msg, game_parameters &parameters) {
    std::vector<std::string> words;
    boost::split(words, msg.data(), boost::is_any_of("\n "));
    // для DEATHMATCH
    /*
    parameters.SetParameters(words[2],
                             std::stoi(words[4]),
                             map_size(std::stoi(words[8]), std::stoi(words[9])),
                             std::stoi(words[11]),
                             std::stoi(words[13]),
                             std::stoi(words[15]),
                             std::stoi(words[17]),
                             std::stoi(words[19]),
                             std::stoi(words[21]));
                             */
    // для FRIENDLY
    parameters.SetParameters("1",
                             std::stoi(words[2]),
                             map_size(std::stoi(words[6]), std::stoi(words[7])),
                             1,
                             std::stoi(words[9]),
                             std::stoi(words[11]),
                             std::stoi(words[13]),
                             std::stoi(words[15]),
                             std::stoi(words[17]));
}
