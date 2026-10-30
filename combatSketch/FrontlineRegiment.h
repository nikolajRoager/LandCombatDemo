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
    int EWUnits_;

    //---Ammunition----
    ///An IFV uses 1 unit of fuel per action, a tank uses 3
    int fuel_;
    ///Standard infantry bullets
    int64_t bullets_;
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

    //Which nation has what recon on this regiment
    std::map<uint32_t,float> nationRecon_;

    FrontlineRegiment(uint32_t tileId, uint32_t ownerId,BattleStats stats) {
        tileId_ = tileId;
        ownerId_ = ownerId;
        soldiers_=stats.std_soldiers_;
        tanks_=stats.std_tanks_;
        IFVs_=stats.std_IFVs_;
        EWUnits_=stats.std_EWUnits_;
        fuel_=stats.std_fuel_;
        bullets_=stats.std_bullets_;
        infantryAntiTank_=stats.std_infantryAntiTank_;
        infantryAntiAir_=stats.std_infantryAntiAir_;
        reconDrones_=stats.std_reconDrones_;
        attackDrones_=stats.std_attackDrones_;
        tankShells_=stats.std_tankShells_;
        entrenchment_=0.f;
        morale_=1.f;
    }

    float getRecon(uint32_t nation)
    {
        return (nationRecon_.contains(nation) ? nationRecon_[nation] : 0.f);
    }

    void reconDecay(BattleStats battleStats) {
        for (auto it = nationRecon_.begin(); it != nationRecon_.end();) {
            it->second = it->second*(1.f-battleStats.reconDecay);
            if (it->second<battleStats.reconRemoval) {
                it = nationRecon_.erase(it);
            }
            else {
                ++it;
            }
        }
    }
};


#endif //LANDCOMBATSKETCH_REGIMENT_H