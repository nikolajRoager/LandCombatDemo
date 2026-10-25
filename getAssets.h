//
// Created by nikolaj on 12/24/25.
//

#ifndef COUNTRYBRAWL_GETASSETS_H
#define COUNTRYBRAWL_GETASSETS_H

#include <filesystem>

namespace fs = std::filesystem;

///Path where assets is stored, defined in a seperate header so we can easily move it elsewhere
inline fs::path assetsPath() {
    return fs::path("assets");
}

#endif //COUNTRYBRAWL_GETASSETS_H