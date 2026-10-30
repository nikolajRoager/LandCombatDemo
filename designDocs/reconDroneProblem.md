I am working on a modern-military themed real-time tactics game in C++.

Land combat in the game is based on regiments, which are located on different tiles, which fight each other in different phases using their stats and rng (quite like the Hearts of Iron series of games).

I would like some help with how I can efficiently get the results of the phases with as few RNG calls as possible.

One phase is drone-recon, where regiment A sends `int NDrones` recon drones against a neighbouring regiment B. Drone recon is split into a spotting phase, an EW phase, and a hard-kill phase.


The first problem is the spotting phase:

Regiment `B` has `int NReadySoldiers` infantry soldiers actively patrolling in an area where they might spot the drone, and `int NRadar` radar

In the spotting phase, the drones `int Stealth` stat is pitted against the `int infantrySpotting` and `int radarSpotting` stats, each soldier rolls a number between 0 and `infantrySpotting` against each drone, which rolls a number between 0 and `Stealth`, if the soldiers roll is higher or equal to the drones roll, the drone is spotted. Similarly all radars roll against all drones.

Now one way to implement this is to literally loop through every soldier-drone pair and every radar-drone pair and actually roll all the random numbers ... But is there a more efficient way of calculating how many drones have been spotted and not?

...

The `int NSpottedReconDrones` drones which have survived the EW phase enter the hard-kill phase, where regiment B tries to destroy them with up to `int maxAvailableAntiAir` anti-air munitions. Each anti-air munition has a  `float antiAir_drone_killChance` chance of killing a drone. Regiment B keeps launching anti-air munitions (one at the time) until  all spotted drones are destroyed or until they run out of munitions.

How can I calculate the number of anti-air munitions spend and the number of drones destroyed?


Regiment B has `B.infantry_` soldiers, `B.bullets_` bullets, `B.infantryAntiAir` infantry launched anti-air ...

For each drone which completes its recon flight