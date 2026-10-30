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

private:
    uint32_t id_;
    BattleStats battleStats_;
    uint8_t red_, green_, blue_;
};


#endif //LANDCOMBATSKETCH_NATION_H