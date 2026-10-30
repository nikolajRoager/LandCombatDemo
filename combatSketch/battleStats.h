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
    float bulletsPerSoldier_ = 100;
    ///How many drones will each soldier on average fire, in other wors I send in the HORDE, I send in the SWARM
    float dronesPerSoldier_=0.3;
    float shellsPerTank_ = 20;
    float shellsPerIFV_ = 10;

    float moralePerSoldier_=1.0;
    float moralePerTank_=2.0;
    float moralePerIFV_=2.0;

    float moraleRecoveryRate_=0.3;

    //Morale lost when a unit is killed
    float moraleLossPerInfantry_=30.0;
    float moraleLossPerTank_=100.0;
    float moraleLossPerIFV_=50.0;

    //Morale lost when taking a step forward, as a fraction of total morale,
    float moraleLossWhenAdvancing_=0.8;
    //What fraction of max morale do we get back when retreating
    float moraleRallyFactor_=0.5;


    //Weights determining what front-line units (infantry, tanks, and IFV) would like to shoot at
    float frontline_targetInfantryWeight_=1.0;
    float frontline_targetTankWeight_=5.0;//Armor attracts a lot of fire
    float frontline_targetIFVWeight_=5.0;
    float frontline_targetEWWeight_=1.0;//EW is a priority target, but tends to be hidden better

    //Chance of disabling each of the four types using bullets
    float bullet_infantryKillRate_=0.005;
    float bullet_tankKillRate_=0.00025;
    float bullet_IFVKillRate_=0.0005;
    float bullet_droneKillRate_=0.005;
    float bullet_EWKillRate_=0.0005;

    //Chance of disabling each of the four types using drones
    float drone_infantryKillRate_=0.05;
    float drone_tankKillRate_=0.0025;//It takes many drones to knock out a tank
    float drone_IFVKillRate_=0.005;
    float drone_EWKillRate_=0.0025;

    //Chance of disabling each of the four types using explosives (tank-shells, artillery, or anti-tank)
    float explosive_infantryKillRate_=0.025;//A hit is a guaranteed kill, but infantry tend to be dug in
    float explosive_tankKillRate_=0.005;//A direct hit or very near-miss is required to disable a tank
    float explosive_IFVKillRate_=0.01;//Less hardened
    float explosive_EWKillRate_=0.005;

    //Interceptor drones and man-pads are highly effective against drones
    float infantry_antiAir_droneKillRate_=0.5;

    ///What factor stored recon decays by each step
    float reconDecay=0.3;
    ///If recon is less than this, we drop it to zero
    float reconRemoval=0.05;
    ///How large a budget does a standard regiment have to launch reconnaissance each recon phase
    int dailyReconPoints_=500;
    int droneReconCost_=1;
    ///How much recon a drone can gain
    float droneReconGain_=0.05;
    uint32_t droneEWDefence_= 10;
    uint32_t EWAttack_=5;

    ///How large a fraction of regiment is, at any time, available to engage any specific enemy recon attempts; represents those which are on guard at the right section of the front
    float counterReconFraction_=0.01;

    ///Each infantryman who sees a drone fires this many bullets (on average), stored as 64 bit to save on conversions
    ///The average is low to represent the fact that most soldiers aren't anywhere near the drone, but those which are near unloads with shotguns or assault riffles
    int64_t averageInfantryBulletsPerReconDroneNominator_=1;
    int64_t averageInfantryBulletsPerReconDroneDenominator_=1;
    ///Each infantryman who sees a drone fires this many anti-air interceptors (catch all term for man-pads or interceptor drones)
    ///The average is very low, since not everyone has anti air ready, and those which do might not see the drone
    int32_t averageInfantryAntiAirPerReconDroneNominator_=1;
    int32_t averageInfantryAntiAirPerReconDroneDenominator_=10;


    //Fighting strengths of standard front-line mechanized infantry regiment
    ///Soldiers in a standard line regiment
    int std_soldiers_=4000;
    ///Number of tanks supporting a standard line regiment
    int std_tanks_=5;
    ///Number of IFVs for a standard line regiment, should be enough to carry everyton
    int std_IFVs_=400;
    int std_EWUnits_=20;
    ///An IFV uses 2 unit of fuel per action, a tank uses 6
    int std_fuel_=4000;
    ///Standard infantry bullets
    int64_t std_bullets_=4000000;
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