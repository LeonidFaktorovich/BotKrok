#include "Algorithm.h"

Square::Square(const SquareType &new_type) : type_(new_type) {
}

Square::Square(const SquareType &new_type, size_t coin) : type_(new_type), coin_number_(coin) {
}

const SquareType& Square::GetType() {
    return type_;
}

size_t Square::GetCoins() {
    return coin_number_;
}

void Field::SetSquare(size_t x, size_t y, Square new_square) {
    table_[x][y] = std::move(new_square);
}

const Square& Field::GetSquare(size_t x, size_t y) {
    return table_[x][y];
}

Algorithm::Algorithm(game_parameters params) {
    params_ = std::move(params);
}

std::pair<size_t, size_t> Algorithm::GetNextStep() {

}
