//
// Created by nikolaj on 10/25/26.
//

#ifndef LANDCOMBATSKETCH_COSTS_H
#define LANDCOMBATSKETCH_COSTS_H
#include <cstdint>

///Cost of an action to one side
struct Cost {
    int32_t soldiersLost=0;
    int32_t IFVsLost=0;
    int32_t tanksLost=0;
    int32_t reconDronesLost=0;
};

struct ReconCost {
    uint32_t targetTile=0;
    float reconGain=0;
    // --- Recon force losses ---
    int32_t soldiersLost=0;
    int32_t IFVsLost=0;
    int32_t tanksLost=0;
    int32_t reconDronesLost=0;

};
#endif //LANDCOMBATSKETCH_COSTS_H