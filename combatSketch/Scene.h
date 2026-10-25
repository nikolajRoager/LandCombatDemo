//
// Created by nikolaj on 2/17/26.
//

#ifndef GLOBEMOVEMENTDEMO_SCENE_H
#define GLOBEMOVEMENTDEMO_SCENE_H
#include <SDL2/SDL_render.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <random>

#include "nation.h"
#include "tile.h"

#include "../inputData.h"
#include "../NumberRenderer.h"
#include "../textureManager.h"
#include "../TexWrap.h"

class Scene {
public:
    Scene(SDL_Renderer* renderer, int screenWidth, int screenHeight, TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont);
    ~Scene();

    void render(SDL_Renderer* renderer, int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis) const;
    void update(SDL_Renderer* renderer, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int dmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont);

private:

    void updateCombat();

    TextureManager textureManager_;
    float cos60_, sin60_;

    std::vector<Nation> nations_;
    std::vector<std::shared_ptr<FrontlineRegiment>> frontlineRegiments_;
    std::vector<Tile> tiles_;
    std::mt19937 rng_;
    NumberRenderer smallNumberRenderer_;

    std::shared_ptr<const TexWrap> regimentTexture_;
};


#endif //GLOBEMOVEMENTDEMO_SCENE_H