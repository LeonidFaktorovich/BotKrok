#include "Bot.h"

int main() {

    Bot my_bot("Kaban", "Baza", MatchMode::DEATHMATCH);
    const char ip[] = "178.216.99.141";
    int port = 2022;
    my_bot.StartSession(ip, port);
    //localhost: 127.0.0.1, port 2021
    //server: 178.216.99.141, port 2022
    // name: Kaban
    // secret: Baza
    return 0;
}
