#pragma once
#include <boost/array.hpp>
#include <string>
#include <vector>

using pair = std::pair<size_t, size_t>;
using message_type = boost::array<char, 15000>;

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

enum class SquareType {
    Coin,
    Block,
    Empty,
    OtherBot,
    MyBot,
    None,
};

class Square
{
 public:
    Square();
    Square(const SquareType &new_type);
    Square(const SquareType &new_type, size_t coin);
    const SquareType &Type() const;
    size_t Coins() const;

 private:
    SquareType type_;
    size_t coin_number_;
};

class Col
{
 public:
    Col(size_t size);
    Square &operator[](size_t y);

 private:
    std::vector<Square> col_;
};

class Field
{
 public:
    Field(const map_size &size);
    void SetSquare(size_t x, size_t y, Square new_square);
    const Square &GetSquare(size_t x, size_t y);
    size_t GetHeight();
    size_t GetWidth();

 private:
    std::vector<Col> table_;
    map_size table_size_;
};

class Algorithm
{
 public:
    Algorithm(const game_parameters &params);
    void Set(size_t x, size_t y, Square new_square);
    pair GetNextStep(const pair &current_position, const std::pair<int, int>& last_step);
    void SetEmptySquare(const pair &current_position);

 private:
    game_parameters params_;
    Field field_;
};
