//
// Created by nikolaj on 10/24/26.
//

#ifndef LANDCOMBATSKETCH_REGIMENT_H
#define LANDCOMBATSKETCH_REGIMENT_H

#include <cstdint>

#include "battleStats.h"

///A front-line regiment holds or takes front-line trenches, does not include support regiments (artillery, airpower, mid-range drones)
struct FrontlineRegiment {

    uint32_t tileId_=0;
    uint32_t ownerId_=0;

    float morale_;
    float entrenchment_;

    //Fighting strengths
    int soldiers_;
    int tanks_;
    int IFVs_;

    //---Ammunition----
    ///An IFV uses 1 unit of fuel per action, a tank uses 3
    int fuel_;
    ///Standard infantry bullets
    int bullets_;
    ///Infantry anti-tank
    int infantryAntiTank_;
    ///Infantry anti-air or anti-drone (man-pads or interceptor drones)
    int infantryAntiAir_;
    ///infantry general purpose
    int attackDrones_;
    ///Infantry recon option
    int reconDrones_;
    ///Tank general purpose
    int tankShells_;

    explicit FrontlineRegiment(uint32_t tileId, uint32_t ownerId,BattleStats stats) {
        tileId_ = tileId;
        ownerId_ = ownerId;
        soldiers_=stats.std_soldiers_;
        tanks_=stats.std_tanks_;
        IFVs_=stats.std_IFVs_;
        fuel_=stats.std_fuel_;
        bullets_=stats.std_bullets_;
        infantryAntiTank_=stats.std_infantryAntiTank_;
        infantryAntiAir_=stats.std_infantryAntiAir_;
        reconDrones_=stats.std_reconDrones_;
        attackDrones_=stats.std_attackDrones_;
        tankShells_=stats.std_tankShells_;

    }
};

#endif //LANDCOMBATSKETCH_REGIMENT_H