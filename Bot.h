#ifndef BOTKROK__BOT_H_
#define BOTKROK__BOT_H_
#include <string>

#include "Algorithm.h"
#include "MsgProcess.h"
#include "SocketSession.h"

class Bot
{
 public:
    Bot(std::string bot_name,
        std::string bot_secret,
        MatchMode match_mode);
    void StartSession(const char* ip, int port);
    ~Bot();

 private:
    SocketSession* socket_session_ = nullptr;
    std::string bot_name_;
    std::string bot_secret_;
    MatchMode match_mode_;
    Algorithm* algorithm = nullptr;
    std::vector<std::pair<int, int>> history_of_step_ = {{-1, -1}};
};

#endif//BOTKROK__BOT_H_
