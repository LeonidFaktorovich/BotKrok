#include "Algorithm.h"
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>

map_size::map_size(size_t width, size_t height) {
    map_height = height;
    map_width = width;
}

void game_parameters::SetParameters(const std::string &match_id,
                                    size_t num_rounds,
                                    const map_size &map,
                                    size_t num_bots,
                                    size_t my_id,
                                    size_t view_radius,
                                    size_t mining_radius,
                                    size_t attack_radius,
                                    size_t move_time_limit) {
    this->match_id = match_id;
    this->num_rounds = num_rounds;
    this->map = map;
    this->num_bots = num_bots;
    this->my_id = my_id;
    this->view_radius = view_radius;
    this->mining_radius = mining_radius;
    this->attack_radius = attack_radius;
    this->move_time_limit = move_time_limit;
}

Square::Square() : type_(SquareType::None) {}

Square::Square(const SquareType &new_type) : type_(new_type) {
}

Square::Square(const SquareType &new_type, size_t coin) : type_(new_type), coin_number_(coin) {
}

const SquareType &Square::Type() const {
    return type_;
}

size_t Square::Coins() const {
    return coin_number_;
}

Field::Field(const map_size &size) : table_size_(size), table_(size.map_width, std::vector<Square>(size.map_height, SquareType::None)) {
}

void Field::SetSquare(size_t x, size_t y, Square new_square) {
    table_[x][y] = std::move(new_square);
}

const Square &Field::GetSquare(size_t x, size_t y) {
    return table_[x][y];
}

size_t Field::GetHeight() {
    return table_size_.map_height;
}

size_t Field::GetWidth() {
    return table_size_.map_width;
}

Algorithm::Algorithm(const game_parameters &params) : params_(params), field_(params.map) {
}

struct HashPair {
    size_t operator()(const pair &p) const {
        return std::hash<size_t>()(p.first) ^ std::hash<size_t>()(p.second);
    }
};

void Algorithm::Set(size_t x, size_t y, Square new_square) {
    field_.SetSquare(x, y, new_square);
}

void Algorithm::SetEmptySquare(const pair &current_position) {
    size_t height = field_.GetHeight();
    size_t width = field_.GetWidth();
    size_t radius = params_.view_radius;

    size_t left_border = (current_position.first + width - radius) % width;
    size_t right_border = (current_position.first + radius) % width;
    size_t lower_border = (current_position.second + height - radius) % height;
    size_t upper_border = (current_position.second + radius) % height;

    for (size_t k = left_border; k != right_border; k = ++k % width) {
        for (size_t l = lower_border; l != upper_border; l = ++l % height) {
            if (std::pow(current_position.first - k, 2) + std::pow(current_position.second - l, 2) <= std::pow(radius, 2) &&
                field_.GetSquare(k, l).Type() == SquareType::None) {
                field_.SetSquare(k, l, Square(SquareType::Empty));
            }
        }
    }
}

pair Algorithm::GetNextStep(const pair &current_position) {
    size_t height = field_.GetHeight();
    size_t width = field_.GetWidth();

    size_t radius_height = std::min(static_cast<size_t>(25), height / 2);
    size_t radius_width = std::min(static_cast<size_t>(25), width / 2);

    size_t left_border = (current_position.first + width - radius_width) % width;
    size_t right_border = (current_position.first + radius_width) % width;
    size_t lower_border = (current_position.second + height - radius_height) % height;
    size_t upper_border = (current_position.second + radius_height) % height;

    std::unordered_set<pair, HashPair> has_coin_nearby;

    for (size_t i = left_border; i != right_border; i = ++i % width) {
        for (size_t j = lower_border; j != upper_border; j = ++j % height) {

            if (field_.GetSquare(i, j).Type() == SquareType::Coin) {
                for (size_t k = left_border; k != right_border; k = ++k % width) {
                    for (size_t l = lower_border; l != upper_border; l = ++l % height) {

                        if (std::pow(i - j, 2) + std::pow(k - l, 2) <= std::pow(params_.mining_radius, 2)) {
                            has_coin_nearby.insert({k, l});
                        }
                    }
                }
            }
        }
    }

    std::unordered_map<pair, pair, HashPair> prev;
    std::queue<pair> q;
    q.push(current_position);

    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    while (!q.empty()) {
        auto now = q.front();
        q.pop();
        for (const auto &neigh : neighbours) {
            pair next = {(static_cast<int>(now.first + width) + neigh.first) % width,
                         (static_cast<int>(now.second + height) + neigh.second) % height};

            const auto &type = field_.GetSquare(next.first, next.second).Type();
            if (!prev.contains(next) && (type == SquareType::Empty || type == SquareType::Coin)) {
                prev[next] = now;
                q.push(next);
            }

            if (has_coin_nearby.contains(next)) {
                while (prev.at(next) != current_position) {
                    next = prev.at(next);
                }
                return next;
            }
        }
    }

    // if no coins are found
    for (const auto &neigh : neighbours) {
        pair next = {(static_cast<int>(current_position.first + width) + neigh.first) % width,
                     (static_cast<int>(current_position.second + height) + neigh.second) % height};
        if (field_.GetSquare(next.first, next.second).Type() != SquareType::Block) {
            return next;
        }
    }
}
