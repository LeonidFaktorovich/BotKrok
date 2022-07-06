#include "MsgProcess.h"
#include <boost/array.hpp>
#include <iostream>
#include <vector>

std::string MsgProcess::HelloAnswer(const std::string &bot_name, const std::string &bot_secret, const std::string &match_mode) {
    std::string result_string =
        std::string("register") + std::string("\n") + std::string("bot_name ") + bot_name + std::string("\n") + std::string("bot_secret ") + bot_secret + std::string("\n") + std::string("mode ") + match_mode + std::string("\n") + std::string("end") + std::string("\n");
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
