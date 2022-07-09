#pragma once
#include <boost/array.hpp>
#include <string>
#include <vector>

using pair = std::pair<int, int>;
using message_type = boost::array<char, 15000>;

struct map_size {
    map_size() = default;
    map_size(int width, int height);
    int map_width;
    int map_height;
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
    Square &operator[](int y);

 private:
    std::vector<Square> col_;
};

class Field
{
 public:
    Field(const map_size &size);
    Col &operator[](int x);
    const map_size& GetSize() const;
    size_t GetDistanceSquare(const std::pair<int, int>& point1, const std::pair<int, int>& point2) const;
 private:
    std::vector<Col> table_;
    map_size table_size_;
};

class IteratorRectangle {
 public:
    IteratorRectangle(Field& field, pair center, pair pos, int radius_width, int radius_height);
    IteratorRectangle& operator++();
    pair operator*();
    bool operator !=(const IteratorRectangle& end) const;
 private:
    Field* field_;
    pair pos;
    int radius_width;
    int radius_height;
    pair center;
};

class BigRectangle {
 public:
    BigRectangle(Field& field, pair center, int radius_width, int radius_height);
    IteratorRectangle begin();
    IteratorRectangle end();
 private:
    Field* field_;
    pair center;
    int radius_width;
    int radius_height;
};

class Algorithm
{
 public:
    Algorithm(const game_parameters &params);
    void Set(int x, int y, Square new_square);
    pair GetNextStep(const pair &current_position, const std::pair<int, int> &last_step);
    void SetEmptySquare(const pair &current_position);

 private:
    game_parameters params_;
    Field field_;
};
