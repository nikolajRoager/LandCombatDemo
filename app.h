//
// Created by nikolaj on 2/17/26.
//

#ifndef GLOBEMOVEMENTDEMO_ENGINE_H
#define GLOBEMOVEMENTDEMO_ENGINE_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "combatSketch/Scene.h"
#include "TexWrap.h"

class app {
public:
    app();
    ~app();
    void run();

private:

    //The window we'll be rendering to
    SDL_Window* window_;

    //The window renderer
    SDL_Renderer* renderer_;

    TTF_Font* smallFont_;
    TTF_Font* midFont_;
    TTF_Font* largeFont_;

    Scene* currentScene_;

    int windowWidthPx_;
    int windowHeightPx_;
};




#endif //GLOBEMOVEMENTDEMO_ENGINE_H