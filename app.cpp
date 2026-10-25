//
// Created by nikolaj on 2/17/26.
//

#include "app.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "getAssets.h"

app::app() {
    window_ = nullptr;
    renderer_ = nullptr;
    smallFont_= nullptr;

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        throw std::runtime_error( "SDL_Init Error: " + std::string( SDL_GetError() ) );
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    windowWidthPx_ = DM.w;
    windowHeightPx_ = DM.h;


    window_ = SDL_CreateWindow( "I can write whatever I want here, and nobody can stop me! HA HA HA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidthPx_, windowHeightPx_, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    if( window_ == nullptr)
        throw std::runtime_error( "Window could not be created! SDL Error:" + std::string( SDL_GetError() ) );
    //Create vsynced renderer for window
    renderer_ = SDL_CreateRenderer( window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( renderer_ == nullptr)
    {
        throw std::runtime_error( "Renderer could not be created! SDL Error: "+ std::string(SDL_GetError()) );
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer_, 0xFF, 0xFF, 0xFF, 0xFF );

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        throw std::runtime_error( "SDL_image could not initialize! SDL Error:" + std::string( SDL_GetError() ) );
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        throw std::runtime_error( "SDL_mixer could not initialize! SDL Error:" + std::string( Mix_GetError() ) );
    }


    if (TTF_Init()==-1) {
        throw std::runtime_error( "SDL_TTF could not initialize! SDL Error:" + std::string( SDL_GetError() ) );
    }

    fs::path path = assetsPath()/"AtkinsonHyperlegible-Regular.ttf";
    smallFont_=TTF_OpenFont( path.string().c_str(), 24 );
    if (smallFont_==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }
    midFont_=TTF_OpenFont( path.string().c_str(), 48 );
    if (midFont_==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }
    largeFont_=TTF_OpenFont( path.string().c_str(), 128);
    if (largeFont_==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }

    currentScene_ = new Scene(renderer_,windowWidthPx_,windowHeightPx_,smallFont_,midFont_,largeFont_);
}

app::~app() {
    std::cout<<"Delete current scene"<<std::endl;
    //Must be called before we destroy the renderer, which it hereby is
    delete currentScene_;

    std::cout<<"Delete renderer"<<std::endl;
    if (renderer_!=nullptr)
        SDL_DestroyRenderer(renderer_);
    std::cout<<"Delete window"<<std::endl;
    if (window_!=nullptr)
        SDL_DestroyWindow(window_);
    std::cout<<"Delete fonts"<<std::endl;
    if (smallFont_!=nullptr)
        TTF_CloseFont(smallFont_);
    if (midFont_!=nullptr)
        TTF_CloseFont(midFont_);
    if (largeFont_!=nullptr)
        TTF_CloseFont(largeFont_);
    std::cout<<"Quit TTF..."<<std::endl;
    TTF_Quit();
    std::cout<<"Quit IMG..."<<std::endl;
    IMG_Quit();
    std::cout<<"Quit SDL"<<std::endl;
    SDL_Quit();
}

void app::run() {
    bool quit = false;

    inputData currentInput;

    uint32_t pmillis=SDL_GetTicks();

    while (!quit) {
        currentInput.scroll=0;

        unsigned int millis = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout<<"Quitting..."<<std::endl;
                quit = true;
            }
            if (event.type == SDL_WINDOWEVENT) {
                switch( event.window.event ) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        windowWidthPx_ = event.window.data1;
                        windowHeightPx_ = event.window.data2;
                        break;
                    default:
                        break;
                }
            }
            if (event.type == SDL_MOUSEWHEEL) {
                currentInput.scroll=event.wheel.y;
            }
            if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState( &currentInput.mouseXPx, &currentInput.mouseYPx );
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentInput.leftMouseDown=true;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentInput.rightMouseDown=true;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentInput.leftMouseDown=false;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentInput.rightMouseDown=false;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentInput.leftPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentInput.rightPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_UP) {
                    currentInput.upPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    currentInput.downPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_z) {
                    currentInput.zoomInPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_x) {
                    currentInput.zoomOutPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_s) {
                    currentInput.sPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_n) {
                    currentInput.nPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_m) {
                    currentInput.mPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_v) {
                    currentInput.vPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_HOME) {
                    currentInput.homePressed=true;
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    currentInput.enterPressed=true;
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentInput.escapePressed=true;
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    currentInput.spacePressed=true;
                }
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentInput.leftPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentInput.rightPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_UP) {
                    currentInput.upPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    currentInput.downPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_z) {
                    currentInput.zoomInPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_x) {
                    currentInput.zoomOutPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_s) {
                    currentInput.sPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_n) {
                    currentInput.nPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_m) {
                    currentInput.mPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_v) {
                    currentInput.vPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_HOME) {
                    currentInput.homePressed=false;
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    currentInput.enterPressed=false;
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentInput.escapePressed=false;
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    currentInput.spacePressed=false;
                }
            }
        }
        SDL_Keymod modState = SDL_GetModState();
        if (modState & KMOD_SHIFT)
            currentInput.shiftPressed=true;
        else
            currentInput.shiftPressed=false;

        if (modState & KMOD_CTRL)
            currentInput.ctrlPressed=true;
        else
            currentInput.ctrlPressed=false;
        //At day 47 of running, millis overflows, just pretend no time passed.
        Uint32 dmillis = millis>pmillis ? millis - pmillis : 0;
        currentScene_->update(renderer_,windowWidthPx_,windowHeightPx_,currentInput,millis,dmillis,smallFont_,midFont_,largeFont_);

        //Black background, shouldn't be seen but won't hurt
        SDL_SetRenderDrawColor(renderer_, 0x50, 0x50, 0x50, 0x00);
        SDL_RenderClear( renderer_ );

        currentScene_->render(renderer_,windowWidthPx_,windowHeightPx_,currentInput,millis,pmillis);

        SDL_RenderPresent( renderer_ );


        pmillis=millis;

        currentInput.prevLeftMouseDown=currentInput.leftMouseDown;
        currentInput.prevRightMouseDown=currentInput.rightMouseDown;
        currentInput.prevEnterPressed=currentInput.enterPressed;
        currentInput.prevEscapePressed=currentInput.escapePressed;
        currentInput.prevSPressed=currentInput.sPressed;
        currentInput.prevVPressed=currentInput.vPressed;
        currentInput.prevSpacePressed=currentInput.spacePressed;
        currentInput.prevHomePressed=currentInput.homePressed;
    }

    std::cout<<"While loop has ended"<<std::endl;
}
