//
// Created by nikolaj on 10/24/26.
//

#include "nation.h"

void Nation::reconDecay() {
    for (auto it = tileRecon.begin(); it != tileRecon.end();) {
        it->second = it->second*(1.f-battleStats_.reconDecay);
        if (it->second<battleStats_.reconRemoval) {
            it = tileRecon.erase(it);
        }
        else {
            ++it;
        }
    }
}
