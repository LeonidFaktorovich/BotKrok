#include "Algorithm.h"

#include <boost/functional/hash.hpp>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

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
    y = (y % static_cast<int>(col_.size()) + static_cast<int>(col_.size())) % static_cast<int>(col_.size());
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
    int radius = params_.view_radius;
    for (const auto &[x, y] : BigRectangle(field_, current_position, radius, radius)) {
        if (field_.GetDistanceSquare(current_position, {x, y}) <= radius * radius) {
            field_[x][y] = Square(SquareType::Empty);
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
            auto type = field_[(static_cast<int>(current_position.first + params_.map.map_width) + x_shift) % params_.map.map_width]
                                         [(static_cast<int>(current_position.second + params_.map.map_height) + y_shift) % params_.map.map_height]
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
    const int SQUARE_WITH_COINS = 100;
    int radius = std::min(SQUARE_WITH_COINS, std::min(height, width) / 2);

    std::unordered_set<pair, boost::hash<pair>> has_coin_nearby;
    for (const auto &[x1, y1] : BigRectangle(field_, current_position, radius, radius)) {
        if (field_[x1][y1].Type() == SquareType::Coin) {
            for (const auto &[x2, y2] : BigRectangle(field_, current_position, radius, radius)) {
                if (field_.GetDistanceSquare({x1, y1}, {x2, y2}) <= params_.mining_radius * params_.mining_radius) {
                    has_coin_nearby.insert({x2, y2});
                }
            }
        }
    }

    std::unordered_map<pair, pair, boost::hash<pair>> prev;
    std::queue<pair> q;
    q.push(current_position);

    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
    pair no_target = {-1, -1};
    while (!q.empty()) {
        auto now = q.front();
        q.pop();
        for (const auto &neigh : neighbours) {
            pair next = {(now.first + width + neigh.first) % width,
                         (now.second + height + neigh.second) % height};

            const auto &type = field_[next.first][next.second].Type();
            if (!prev.contains(next) && (type == SquareType::Empty || type == SquareType::Coin)) {
                prev[next] = now;
                q.push(next);
                if (has_coin_nearby.contains(next)) {
                    while (prev.at(next) != current_position) {
                        next = prev.at(next);
                    }
                    block_target_ = no_target;
                    return next;
                }
            }
        }
    }

    pair no_result = {-1, -1};
    auto nearest_coin = GetNearestCoin(current_position);
    if (nearest_coin != no_result) {
        auto next_step = GoToSquare(current_position, nearest_coin);
        if (next_step != no_result) {
            block_target_ = no_target;
            return next_step;
        }
    }
    if (block_target_ == no_target || block_target_ == current_position) {
        block_target_ = GetNearestNoneBlock(current_position);
    }
    if (block_target_ != no_result) {
        auto next_step = GoToSquare(current_position, block_target_);
        if (next_step != no_result) {
            //std::cout << block_target_.first << ' ' << block_target_.second << std::endl;
            return next_step;
        }
    }
    return GetRandomSquare(current_position, last_step);
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

BigRectangle::BigRectangle(Field &field, pair center, int radius_width, int radius_height) : field_(&field),
                                                                                             radius_width(radius_width),
                                                                                             radius_height(radius_height) {
    int x = (center.first % field_->GetSize().map_width + field_->GetSize().map_width) % field_->GetSize().map_width;
    int y = (center.second % field_->GetSize().map_height + field_->GetSize().map_height) % field_->GetSize().map_height;
    this->center.first = x;
    this->center.second = y;
}

IteratorRectangle BigRectangle::begin() {
    return IteratorRectangle(*field_, center, {center.first - radius_width, center.second - radius_height}, radius_width, radius_height);
}

IteratorRectangle BigRectangle::end() {
    return IteratorRectangle(*field_, center, {center.first + radius_width + 1, center.second - radius_height}, radius_width, radius_height);
}

IteratorRectangle::IteratorRectangle(Field &field, pair center, pair pos, int radius_width, int radius_height) : field_(&field),
                                                                                                                 pos(std::move(pos)),
                                                                                                                 radius_width(radius_width),
                                                                                                                 radius_height(radius_height),
                                                                                                                 center(std::move(center)) {
}

bool IteratorRectangle::operator!=(const IteratorRectangle &end) const {
    return std::tie(field_, pos.first, pos.second) != std::tie(end.field_, end.pos.first, end.pos.second);
}

IteratorRectangle &IteratorRectangle::operator++() {
    if ((pos.second - (center.second + radius_height)) == 0) {
        pos.second = center.second - radius_height;
        pos.first += 1;
        return *this;
    }
    pos.second += 1;
    return *this;
}

pair IteratorRectangle::operator*() {
    int x = (pos.first % field_->GetSize().map_width + field_->GetSize().map_width) % field_->GetSize().map_width;
    int y = (pos.second % field_->GetSize().map_height + field_->GetSize().map_height) % field_->GetSize().map_height;
    return {x, y};
}

pair Algorithm::GoToSquare(const pair &current_position, const pair &finish) {
    std::unordered_map<pair, pair, boost::hash<pair>> prev;
    pair no_way = {-1, -1};
    std::queue<pair> q;
    q.push(current_position);

    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
    int height = field_.GetSize().map_height;
    int width = field_.GetSize().map_width;
    while (!q.empty()) {
        auto now = q.front();
        q.pop();
        for (const auto &neigh : neighbours) {
            pair next = {(now.first + width + neigh.first) % width,
                         (now.second + height + neigh.second) % height};

            const auto &type = field_[next.first][next.second].Type();
            if (!prev.contains(next) && (type == SquareType::Empty || type == SquareType::Coin || type == SquareType::None)) {
                prev[next] = now;
                q.push(next);
            }
        }
    }
    if (!prev.contains(finish)) {
        return no_way;
    }
    pair last = finish;
    while (prev.at(last) != current_position) {
        last = prev.at(last);
    }
    return last;
}

pair Algorithm::GetNearestCoin(const pair &current_position) {
    pair no_coins = {-1, -1};
    pair nearest_coin = no_coins;
    size_t min_distance_square = field_.GetDistanceSquare(current_position, no_coins);

    for (size_t x = 0; x < field_.GetSize().map_width; ++x) {
        for (size_t y = 0; y < field_.GetSize().map_height; ++y) {
            if (field_[x][y].Type() == SquareType::Coin &&
                (nearest_coin == no_coins || field_.GetDistanceSquare(current_position, {x, y}) < min_distance_square)) {
                min_distance_square = field_.GetDistanceSquare(current_position, {x, y});
                nearest_coin = {x, y};
            }
        }
    }
    return nearest_coin;
}

//need fix

pair Algorithm::GetNearestNoneBlock(const pair &current_position) {
    std::unordered_set<pair, boost::hash<std::pair<int, int>>> centers;
    std::queue<pair> q;
    pair no_block = {-1, -1};
    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
    const int area_length = 30;
    int current_width = field_.GetSize().map_width - (field_.GetSize().map_width % area_length);
    int current_height = field_.GetSize().map_height - (field_.GetSize().map_height % area_length);
    int new_my_x = (current_position.first >= current_width) ? (current_width - 1) : (current_position.first);
    int new_my_y = (current_position.second >= current_height) ? (current_height - 1) : (current_position.second);
    int center_x = (new_my_x / area_length) * area_length + area_length / 2;
    int center_y = (new_my_y / area_length) * area_length + area_length / 2;
    q.push({center_x, center_y});
    while (!q.empty()) {
        auto [cur_x, cur_y] = q.front();
        q.pop();
        for (const auto& [x, y] : neighbours) {
            int new_x = ((cur_x + area_length * x) % current_width + current_width) % current_width;
            int new_y = ((cur_y + area_length * y) % current_height + current_height) % current_height;
            pair next = {new_x, new_y};
            if (field_[new_x][new_y].Type() == SquareType::None) {
                return next;
            }
            if (!centers.contains(next)) {
                q.push(next);
                centers.insert(next);
            }
        }
    }
    return no_block;
}

pair Algorithm::GetRandomSquare(const pair &current_position, const pair &last_step) {
    std::vector<std::pair<int, int>> neighbours{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
    int width = field_.GetSize().map_width;
    int height = field_.GetSize().map_height;
    size_t last_step_ind = 0;
    while (neighbours[last_step_ind] != last_step) {
        ++last_step_ind;
    }
    bool only_back = false;
    for (size_t index = last_step_ind;; index = (index + 1) % neighbours.size()) {
        if (index == (last_step_ind + 4) % neighbours.size()) {
            if (only_back) {
                return {(static_cast<int>(current_position.first + width) + neighbours[(last_step_ind + 4) % neighbours.size()].first) % width,
                        (static_cast<int>(current_position.second + height) + neighbours[(last_step_ind + 4) % neighbours.size()].second) % height};
            }
            only_back = true;
            continue;
        }
        pair next = {(static_cast<int>(current_position.first + width) + neighbours[index].first) % width,
                     (static_cast<int>(current_position.second + height) + neighbours[index].second) % height};
        if (field_[next.first][next.second].Type() != SquareType::Block) {
            return next;
        }
    }
}