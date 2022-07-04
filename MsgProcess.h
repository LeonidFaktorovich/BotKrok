#ifndef BOTKROK__MSGPROCESS_H_
#define BOTKROK__MSGPROCESS_H_
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <string>

using message_type = boost::array<char, 4096>;

struct map_size {
    map_size() = default;
    map_size(size_t width, size_t height);
    size_t map_width;
    size_t map_height;
};

struct game_parameters {
    game_parameters() = default;
    void SetParameters(const std::string &match_id,
                       size_t num_rounds,
                       const map_size &map,
                       size_t num_bots,
                       size_t my_id,
                       size_t view_radius,
                       size_t mining_radius,
                       size_t attack_radius,
                       size_t move_time_limit);
    std::string match_id;
    size_t num_rounds;
    map_size map;
    size_t num_bots;
    size_t my_id;
    size_t view_radius;
    size_t mining_radius;
    size_t attack_radius;
    size_t move_time_limit;
};
class MsgProcess
{
 public:
    MsgProcess(std::string bot_name,
               std::string bot_secret,
               std::string match_mode);
    std::string GetAnswer(const message_type &msg);
    std::string HelloAnswer();
    const game_parameters GetParameters() const;

 private:
    std::string bot_name_;
    std::string bot_secret_;
    std::string match_mode_;
    size_t protocol_version_;
    game_parameters parameters_;
};

#endif//BOTKROK__MSGPROCESS_H_
