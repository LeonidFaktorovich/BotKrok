#ifndef BOTKROK__MSGPROCESS_H_
#define BOTKROK__MSGPROCESS_H_
#include "Algorithm.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <string>

class MsgProcess
{
 public:
    static std::string HelloAnswer(const std::string &bot_name, const std::string &bot_secret, const std::string &match_mode);
    static void GetParameters(const message_type &msg, game_parameters &parameters);
};

#endif//BOTKROK__MSGPROCESS_H_
