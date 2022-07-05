#ifndef BOTKROK__BOT_H_
#define BOTKROK__BOT_H_
#include <string>
#include "SocketSession.h"
#include "MsgProcess.h"
#include "Algorithm.h"

class Bot {
 public:
    Bot(std::string bot_name,
        std::string bot_secret,
        std::string match_mode);
    void StartSession(const char* ip, int port);
    ~Bot();
 private:
    SocketSession* socket_session_ = nullptr;
    std::string bot_name_;
    std::string bot_secret_;
    std::string match_mode_;
    Algorithm* algorithm = nullptr;
};

#endif //BOTKROK__BOT_H_
