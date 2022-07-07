#include "Algorithm.h"

#include <boost/functional/hash.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

map_size::map_size(int width, int height) {
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

Square::Square(const SquareType &new_type, size_t coin) : type_(new_type),
                                                          coin_number_(coin) {
}

const SquareType &Square::Type() const {
    return type_;
}

size_t Square::Coins() const {
    return coin_number_;
}

Col::Col(size_t size) : col_(size) {
}

Square &Col::operator[](int y) {
    y = (y % col_.size() + col_.size()) % col_.size();
    return col_[y];
}

Field::Field(const map_size &size) : table_size_(size),
                                     table_(size.map_width, Col(size.map_height)) {
}

Col &Field::operator[](int x) {
    x = (x % table_size_.map_width + table_size_.map_width) % table_size_.map_width;
    return table_[x];
}

const map_size &Field::GetSize() const {
    return table_size_;
}

Algorithm::Algorithm(const game_parameters &params) : params_(params),
                                                      field_(params.map) {
}

void Algorithm::Set(int x, int y, Square new_square) {
    field_[x][y] = std::move(new_square);
}

void Algorithm::SetEmptySquare(const pair &current_position) {
    int width = field_.GetSize().map_width;
    int height = field_.GetSize().map_height;
    int radius = params_.view_radius;

    int x_pos = static_cast<int>(current_position.first);
    int y_pos = static_cast<int>(current_position.second);

    int left_bord = (x_pos - radius) % width;
    int right_bord = (x_pos + radius) % width;
    int lower_bord = (y_pos - radius) % height;
    int upper_bord = (y_pos + radius) % height;

    for (int x = left_bord; x != right_bord; x = (x + 1) % width) {
        for (int y = lower_bord; y != upper_bord; y = (y + 1) % height) {
            if (field_.GetDistanceSquare(current_position, {x, y}) <= radius * radius) {
                field_[x][y] = Square(SquareType::Empty);
            }
        }
    }
}

pair Algorithm::GetNextStep(const pair &current_position, const std::pair<int, int> &last_step) {
    /*
    std::cout << "Cur_pos: " << current_position.first << ' ' << current_position.second << std::endl;
    for (int y_shift = 2; y_shift >= -2; --y_shift) {
        for (int x_shift = -2; x_shift <= 2; ++x_shift) {
            if (y_shift == 0 && x_shift == 0) {
                std::cout << 'I';
                continue;
            }
            auto type = field_.GetSquare((static_cast<int>(current_position.first + params_.map.map_width) + x_shift) % params_.map.map_width,
                                         (static_cast<int>(current_position.second + params_.map.map_height) + y_shift) % params_.map.map_height)
                            .Type();
            if (type == SquareType::None) {
                std::cout << 'N';
            } else if (type == SquareType::Coin) {
                std::cout << '*';
            } else if (type == SquareType::Block) {
                std::cout << '.';
            } else if (type == SquareType::Empty) {
                std::cout << '-';
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    */

    int height = field_.GetSize().map_height;
    int width = field_.GetSize().map_width;
    int radius = std::min(25, std::min(height, width) / 2);

    int x_pos = static_cast<int>(current_position.first);
    int y_pos = static_cast<int>(current_position.second);

    int left_bord = (x_pos - radius) % width;
    int right_bord = (x_pos + radius) % width;
    int lower_bord = (y_pos - radius) % height;
    int upper_bord = (y_pos + radius) % height;

    std::unordered_set<pair, boost::hash<pair>> has_coin_nearby;

    for (int x1 = left_bord; x1 != right_bord; x1 = (x1 + 1) % width) {
        for (int y1 = lower_bord; y1 != upper_bord; y1 = (y1 + 1) % height) {
            if (field_[x1][y1].Type() == SquareType::Coin) {
                for (int x2 = left_bord; x2 != right_bord; x2 = (x2 + 1) % width) {
                    for (int y2 = lower_bord; y2 != upper_bord; y2 = (y2 + 1) % height) {
                        if (field_.GetDistanceSquare({x1, y1}, {x2, y2}) <= std::pow(params_.mining_radius, 2)) {
                            has_coin_nearby.insert({(x2 + width) % width, (y2 + height) % height});
                        }
                    }
                }
            }
        }
    }

    std::unordered_map<pair, pair, boost::hash<pair>> prev;
    std::queue<pair> q;
    q.push(current_position);

    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    while (!q.empty()) {
        auto now = q.front();
        q.pop();
        for (const auto &neigh : neighbours) {
            pair next = {(static_cast<int>(now.first + width) + neigh.first) % width,
                         (static_cast<int>(now.second + height) + neigh.second) % height};

            const auto &type = field_[next.first][next.second].Type();
            if (!prev.contains(next) && (type == SquareType::Empty || type == SquareType::Coin)) {
                prev[next] = now;
                q.push(next);
                if (has_coin_nearby.contains(next)) {
                    while (prev.at(next) != current_position) {
                        next = prev.at(next);
                    }
                    return next;
                }
            }
        }
    }

    // if no coins are found
    //boost::range::random_shuffle(neighbours);

    size_t last_step_ind = 0;
    while (neighbours[last_step_ind] != last_step) {
        ++last_step_ind;
    }
    for (size_t index = last_step_ind;; index = (index + 1) % neighbours.size()) {
        if (index == (last_step_ind + 4) % neighbours.size()) {
            continue;
        }
        std::cout << "No coins" << std::endl;
        pair next = {(static_cast<int>(current_position.first + width) + neighbours[index].first) % width,
                     (static_cast<int>(current_position.second + height) + neighbours[index].second) % height};
        std::cout << next.first << ' ' << next.second << std::endl;
        if (field_[next.first][next.second].Type() != SquareType::Block) {
            return next;
        }
    }
    return {(static_cast<int>(current_position.first + width) + neighbours[(last_step_ind + 4) % neighbours.size()].first) % width,
            (static_cast<int>(current_position.second + height) + neighbours[(last_step_ind + 4) % neighbours.size()].second) % height};
    /*
    for (const auto &neigh : neighbours) {
        std::cout << "No coins" << std::endl;
        pair next = {(static_cast<int>(current_position.first + width) + neigh.first) % width,
                     (static_cast<int>(current_position.second + height) + neigh.second) % height};
        std::cout << next.first << ' ' << next.second << std::endl;
        if (field_.GetSquare(next.first, next.second).Type() != SquareType::Block) {
            return next;
        }
    }
     */
}

size_t Field::GetDistanceSquare(const std::pair<int, int> &point1, const std::pair<int, int> &point2) const {
    int width = static_cast<int>(table_size_.map_width);
    int height = static_cast<int>(table_size_.map_height);
    size_t standard_x_diff = std::abs((point2.first % width + width) % width - (point1.first % width + width) % width);
    size_t standard_y_diff = std::abs((point2.second % height + height) % height - (point1.second % height + height) % height);
    size_t x_diff_square = std::min(standard_x_diff, width - standard_x_diff) * std::min(standard_x_diff, width - standard_x_diff);
    size_t y_diff_square = std::min(standard_y_diff, height - standard_y_diff) * std::min(standard_y_diff, height - standard_y_diff);
    return x_diff_square + y_diff_square;
}