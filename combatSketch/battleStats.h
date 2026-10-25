//
// Created by nikolaj on 10/24/26.
//

#ifndef LANDCOMBATSKETCH_BATTLESTATS_H
#define LANDCOMBATSKETCH_BATTLESTATS_H
#include <cstdint>

struct BattleStats {


    ///Chance a bullet attack against a tank gets converted to anti-tank when targeting tanks or ifv
    ///This chance represents the fact that not every soldier carries anti-tank (even if overall stockpiles are available)
    float infantryAntitankChance=0.5;

    ///How many bullets a single soldier on average wants to fire per day
    float bulletsPerSoldier = 1000;
    ///How many drones will each soldier on average fire, in other wors I send in the HORDE, I send in the SWARM
    float dronesPerSoldier=0.3;
    float shellsPerTank = 20;
    float shellsPerIFV = 10;

    float moralePerSoldier=1.0;
    float moralePerTank=2.0;
    float moralePerIFV=2.0;

    float moraleRecoveryRate=0.3;

    //Morale lost when a unit is killed
    float moraleLossPerInfantry=30.0;
    float moraleLossPerTank=100.0;
    float moraleLossPerIFV=50.0;

    //Morale lost when taking a step forward, as a fraction of total morale,
    float moraleLossWhenAdvancing=0.8;
    //What fraction of max morale do we get back when retreating
    float moraleRallyFactor=0.5;


    //Weights determining what front-line units (infantry, tanks, and IFV) would like to shoot at
    float frontline_targetInfantryWeight=1.0;
    float frontline_targetTankWeight=5.0;//Armor attracts a lot of fire
    float frontline_targetIFVWeight=5.0;

    //Weights determining what support (artillery, jets, cruise-missiles) would like to shoot at
    float support_targetInfantryWeight=1.0;
    float support_targetTankWeight=4.0;
    float support_targetIFVWeight=2.0;

    //Chance of disabling each of the four types using bullets
    float bullet_infantryKillRate=0.00005;
    float bullet_tankKillRate=0.0000025;
    float bullet_IFVKillRate=0.000005;

    //Chance of disabling each of the four types using drones
    float drone_infantryKillRate=0.05;
    float drone_tankKillRate=0.005;//It takes many drones to knock out a tank
    float drone_IFVKillRate=0.01;

    //Chance of disabling each of the four types using explosives (tank-shells, artillery, or anti-tank)
    float explosive_infantryKillRate=0.025;//A hit is a guaranteed kill, but infantry tend to be dug in
    float explosive_tankKillRate=0.005;//A direct hit or very near-miss is required to disable a tank
    float explosive_IFVKillRate=0.01;//Less hardened

    ///What factor stored recon decays by each step
    float reconDecay=0.3;
    ///If recon is less than this, we drop it to zero
    float reconRemoval=0.05;
    ///How large a budget does a standard regiment have to launch reconnaissance each recon phase
    int dailyReconPoints_=500;
    int droneReconCost_=1;
    ///How much recon each step of the drone recon gains
    float droneReconGain_=0.05;
    ///How many steps does a drone gain recon over (an the enemy have time to shoot it down)
    int droneReconSteps_=16;

    //Fighting strengths of standard front-line mechanized infantry regiment
    ///Soldiers in a standard line regiment
    int std_soldiers_=4000;
    ///Number of tanks supporting a standard line regiment
    int std_tanks_=5;
    ///Number of IFVs for a standard line regiment, should be enough to carry everyton
    int std_IFVs_=400;
    ///An IFV uses 1 unit of fuel per action, a tank uses 3
    int std_fuel_=4000;
    ///Standard infantry bullets
    int std_bullets_=400000;
    ///Infantry anti-tank
    int std_infantryAntiTank_=4000;
    ///Infantry anti-air or anti-drone (man-pads or interceptor drones)
    int std_infantryAntiAir_=4000;
    ///infantry general purpose
    int std_attackDrones_=1000;
    ///Infantry recon option
    int std_reconDrones_=500;
    ///Tank and IFV general purpose
    int std_tankShells_=45000;
};

#endif //LANDCOMBATSKETCH_BATTLESTATS_H