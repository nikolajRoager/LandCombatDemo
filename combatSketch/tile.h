//
// Created by nikolaj on 10/24/26.
//

#ifndef LANDCOMBATSKETCH_TILE_H
#define LANDCOMBATSKETCH_TILE_H
#include <cmath>
#include <list>
#include <memory>
#include <vector>

#include "FrontlineRegiment.h"

struct Tile {
    uint32_t ownerId_;
    float x_,y_;
    float radius_;
    std::vector<uint32_t> neighbourIds_;
    std::vector<std::shared_ptr<FrontlineRegiment>> frontlineRegiments_;

    Tile(float x, float y, float radius, uint32_t ownerId) : x_(x), y_(y), radius_(radius), ownerId_(ownerId) {}

    ///Change this function if the tiles are projected onto a globe
    [[nodiscard]] inline float dist(const Tile &other) const {
        return std::sqrt(std::pow(other.x_ - x_, 2.f) + std::pow(other.y_ - y_, 2.f));
    }
};

#endif //LANDCOMBATSKETCH_TILE_H