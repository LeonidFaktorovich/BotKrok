#include "MsgProcess.h"

#include <boost/array.hpp>
#include <vector>

std::string MsgProcess::HelloAnswer(const std::string &bot_name, const std::string &bot_secret, const std::string &match_mode) {
    std::string result_string =
        std::string("register") + std::string("\n") + std::string("bot_name ") + bot_name + std::string("\n") + std::string("bot_secret ") + bot_secret + std::string("\n") + std::string("mode ") + match_mode + std::string("\n") + std::string("end") + std::string("\n");
    return result_string;
}

bool MsgProcess::GetParameters(const message_type &msg, game_parameters &parameters, MatchMode match_mode) {
    std::vector<std::string> words;
    boost::split(words, msg.data(), boost::is_any_of("\n "));
    if (match_mode == MatchMode::DEATHMATCH) {
        parameters.SetParameters("1",
                                 std::stoi(words[2]),
                                 map_size(std::stoi(words[6]), std::stoi(words[7])),
                                 2,
                                 std::stoi(words[9]),
                                 std::stoi(words[11]),
                                 std::stoi(words[13]),
                                 std::stoi(words[15]),
                                 std::stoi(words[17]));
    }

    else if (match_mode == MatchMode::FRIENDLY) {
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
    const size_t count_of_words = 20;
    if (words.size() != count_of_words) {
        return true;
    }
    return false;
}

std::pair<int, int> MsgProcess::GetData(const message_type &msg, size_t my_id, Algorithm &algorithm) {
    std::vector<std::string> cells;
    std::pair<int, int> no_info = {-1, -1};
    std::pair<int, int> my_pos = no_info;
    boost::split(cells, msg.data(), boost::is_any_of("\n"));

    size_t update_index = 0;
    const std::string update = "update";

    while (update_index < cells.size() && cells[update_index] != update) {
        ++update_index;
    }
    for (size_t i = update_index + 2; i + 1 < cells.size(); ++i) {
        std::vector<std::string> cur_data;
        boost::split(cur_data, cells[i], boost::is_any_of(" "));
        if (cur_data[0] == "bot" && std::stoi(cur_data[4]) == my_id) {
            my_pos.first = std::stoi(cur_data[1]);
            my_pos.second = std::stoi(cur_data[2]);
        }
    }
    if (my_pos != no_info) {
        algorithm.SetEmptySquare(my_pos);
    }
    for (size_t i = 2; i + 1 < cells.size(); ++i) {
        std::vector<std::string> cur_data;
        boost::split(cur_data, cells[i], boost::is_any_of(" "));
        if (cur_data[0] == "end") {
            break;
        }
        if (cur_data[0] == "coin") {
            algorithm.Set(std::stoi(cur_data[1]), std::stoi(cur_data[2]), Square(SquareType::Coin));

        } else if (cur_data[0] == "block") {
            algorithm.Set(std::stoi(cur_data[1]), std::stoi(cur_data[2]), Square(SquareType::Block));
        } else if (cur_data[0] == "bot") {
            if (std::stoi(cur_data[4]) != my_id) {
                algorithm.Set(std::stoi(cur_data[1]), std::stoi(cur_data[2]), Square(SquareType::OtherBot, std::stoi(cur_data[3])));
            }
        }
    }
    return my_pos;
}