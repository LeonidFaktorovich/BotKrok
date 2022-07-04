#include "MsgProcess.h"

enum class SquareState {
    Coin,
    Block,
    Empty,
    OtherBot,
    MyBot
};

class Field
{
 public:
    Field() = default;
    void Set(size_t x, size_t y, SquareState new_state);
    SquareState Get(size_t x, size_t y);

 private:
    std::vector<std::vector<SquareState>> table;
    map_size table_size;
};

class Algorithm
{
 public:
    Algorithm() = default;

 private:
    game_parameters params;
    Field field;
};
