#ifndef BOTKROK__MSGPROCESS_H_
#define BOTKROK__MSGPROCESS_H_
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <string>

#include "Algorithm.h"

class MsgProcess
{
 public:
    static std::string HelloAnswer(const std::string &bot_name, const std::string &bot_secret, const std::string &match_mode);
    static void GetParameters(const message_type &msg, game_parameters &parameters);
    static std::pair<size_t, size_t> GetData(const message_type &msg, size_t my_id, Algorithm &algorithm);
};

#endif//BOTKROK__MSGPROCESS_H_
