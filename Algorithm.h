#pragma once
#include "MsgProcess.h"

enum class SquareType {
    Coin,
    Block,
    Empty,
    OtherBot,
    MyBot
};

class Square {
 public:
    Square(const SquareType &new_type);
    Square(const SquareType &new_type, size_t coin);
    const SquareType& GetType();
    size_t GetCoins();

 private:
    SquareType type_;
    size_t coin_number_;
};

class Field
{
 public:
    Field() = default;
    void SetSquare(size_t x, size_t y, Square new_square);
    const Square& GetSquare(size_t x, size_t y);

 private:
    std::vector<std::vector<Square>> table_;
    map_size table_size_;
};

class Algorithm
{
 public:
    Algorithm(game_parameters params);
    void Set(size_t x, size_t y, Square new_square);
    std::pair<size_t, size_t> GetNextStep();

 private:
    game_parameters params_;
    Field field_;
};
