//
// Created by nikolaj on 10/24/26.
//

#ifndef LANDCOMBATSKETCH_NATION_H
#define LANDCOMBATSKETCH_NATION_H
#include <cstdint>
#include <map>

#include "battleStats.h"


class Nation {
public:
    [[nodiscard]] uint8_t getRed() const {return red_;}
    [[nodiscard]] uint8_t getBlue() const {return blue_;}
    [[nodiscard]] uint8_t getGreen() const {return green_;}
    [[nodiscard]] uint32_t getId() const {return id_;}
    [[nodiscard]] const BattleStats& getBattleStats() const {return battleStats_;}
    explicit Nation(uint32_t id, uint8_t r, uint8_t g, uint8_t b): id_(id), battleStats_(), red_(r), blue_(b), green_(g) {}

    float getRecon(uint32_t tileId) {
        return tileRecon.count(tileId) ? tileRecon[tileId] : 0.0f;
    }

    void reconDecay();

private:
    uint32_t id_;
    BattleStats battleStats_;
    uint8_t red_, green_, blue_;

    ///Reconnaissance (0.0 to 1.0) for each tile, if the tile is not present then it is 0
    std::map<uint32_t,float> tileRecon;
};


#endif //LANDCOMBATSKETCH_NATION_H