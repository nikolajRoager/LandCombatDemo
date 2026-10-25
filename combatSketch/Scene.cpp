//
// Created by nikolaj on 2/17/26.
//

#include "Scene.h"

#include <chrono>
#include <complex>
#include <fstream>
#include <iostream>

#include "../getAssets.h"
#include "../ThreadPool.h"


//Safely draw from a binomial distribution. Guards against:
// - n <= 0 (no trials to run)
// - p being NaN/inf (from a 0/0 division upstream)
// - p drifting slightly outside [0,1] from floating-point rounding
inline int32_t safeBinomial(int32_t n, float p, std::mt19937& generator) {
    if (n <= 0) return 0;
    if (!std::isfinite(p) || p <= 0.0f) return 0;
    if (p > 1.0f) p = 1.0f; //clamp rounding error, e.g. 1.0000000002
    std::binomial_distribution<int32_t> dist(n, p);
    return dist(generator);
}

//Safe division for building weights/probabilities. Returns 0 if there's
//nothing to divide by (e.g. defender category is fully wiped out), and
//clamps the result to a valid probability range as a final safety net.
inline float safeDiv(float numerator, float denominator) {
    if (denominator <= 0.0f || !std::isfinite(denominator)) return 0.0f;
    const float result = numerator / denominator;
    if (!std::isfinite(result)) return 0.0f;
    return std::clamp(result, 0.0f, 1.0f);
}

Scene::Scene(SDL_Renderer *renderer,int screenWidth, int screenHeight, TTF_Font *smallFont, TTF_Font *midFont, TTF_Font *largeFont):
rng_(0),
smallNumberRenderer_(0,smallFont,renderer)
{
    //First launch the threadpool and start loading everything
    //Using a threadpool is CRAZY overkill for this project,
    std::vector<fs::path> textureRequests
    {
        "regiment.png"
    };

    ThreadPool loadingPool(std::thread::hardware_concurrency()-1);

    textureManager_.launchTextureLoading(textureRequests, assetsPath(),loadingPool);

    cos60_=std::cos(60.0*M_PI/180.0);
    sin60_=std::sin(60.0*M_PI/180.0);

    double radius = 64;
    int hexesX = 16;
    int hexesY = 10;

    for (int yi = 0; yi < hexesY; yi++) {
        for (int xi = 0; xi < hexesX; xi++) {
            tiles_.emplace_back(xi*2*radius*sin60_+((yi%2)*radius*sin60_)+radius,yi*radius*(1+cos60_)+radius,radius, xi<hexesX/2 ? 0 : 1);
            if (xi>0)
                tiles_.back().neighbourIds_.emplace_back(xi-1 + yi*hexesX);
            if (xi+1< hexesX)
                tiles_.back().neighbourIds_.emplace_back(xi+1 + yi*hexesX);
            if (yi%2 == 0) {
                if (yi>0) {
                    tiles_.back().neighbourIds_.emplace_back(xi + (yi-1)*hexesX);
                    if (xi>0)
                        tiles_.back().neighbourIds_.emplace_back(xi-1 + (yi-1)*hexesX);
                }
                if (yi+1<hexesY) {
                    tiles_.back().neighbourIds_.emplace_back(xi + (yi+1)*hexesX);
                    if (xi>0)
                        tiles_.back().neighbourIds_.emplace_back(xi-1 + (yi+1)*hexesX);
                }
            }
            else {
                if (yi>0) {
                    tiles_.back().neighbourIds_.emplace_back(xi + (yi-1)*hexesX);
                    if (xi+1<hexesX)
                        tiles_.back().neighbourIds_.emplace_back(xi+1 + (yi-1)*hexesX);
                }
                if (yi+1<hexesY) {
                    tiles_.back().neighbourIds_.emplace_back(xi + (yi+1)*hexesX);
                    if (xi+1<hexesX)
                        tiles_.back().neighbourIds_.emplace_back(xi+1 + (yi+1)*hexesX);
                }
            }
        }
    }

    nations_=std::vector<Nation>{
    Nation(0,0,0,255),
    Nation(1,255,0,0),
    Nation(2,0,255,0),};

    //Add regiments to front-line tiles
    for (int yi = 0; yi < hexesY; yi++)
        for (int xi = 0; xi < hexesX; xi++) {

            auto& tile = tiles_[yi*hexesX+xi];
            bool frontline=false;
            for (uint32_t i : tile.neighbourIds_) {
                if (tiles_[i].ownerId_!=tile.ownerId_) {
                    frontline=true;
                    break;
                }
            }
            if (frontline) {
                std::uniform_int_distribution<> distribution(1,3);
                size_t nRegiments = distribution(rng_);
                for (size_t i = 0; i < nRegiments; i++) {
                    auto ptr = std::make_shared<FrontlineRegiment>(yi*hexesX+xi,tile.ownerId_, nations_[tile.ownerId_].getBattleStats());
                    frontlineRegiments_.push_back(ptr);
                    tile.frontlineRegiments_.push_back(ptr);
                }
            }
    }


    //==Finally let the loading of images finish with a loading bar (might not be needed)==

    //Then display a loading bar
    while (textureManager_.getLoadedAssets()< textureManager_.getAssetsToLoad()&& !textureManager_.isCanceled()) {
        //Respond to window resize events;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        screenWidth= event.window.data1;
                        screenHeight= event.window.data2;
                        break;
                    default:
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_Rect loadingBarRect = {
            0, screenHeight/ 2, static_cast<int>((screenWidth* textureManager_.getLoadedAssets()) / textureManager_.getAssetsToLoad()), screenHeight/ 4
        };
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

        SDL_RenderFillRect(renderer, &loadingBarRect);

        SDL_RenderPresent(renderer);
    }

    if (textureManager_.isCanceled()) {
        throw std::runtime_error("The loading cancelled with the following error: " + textureManager_.getErrorMessage());
    }

    std::cout << "Finalizing" << std::endl;
    textureManager_.uploadTexturesToGPU(renderer);
    regimentTexture_=textureManager_.getTexWrap("regiment.png");
}

Scene::~Scene() = default;

void Scene::update(SDL_Renderer *renderer, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int dmillis, TTF_Font *smallFont, TTF_Font *midFont, TTF_Font *largeFont) {
    if (userInputs.enterPressed && !userInputs.prevEnterPressed) {
        updateCombat();
    }
}

void Scene::updateCombat() {
    std::cout<<"Update combat"<<std::endl;

    std::cout<<"Status update"<<std::endl;
    for (auto& nation : nations_) {
        nation.reconDecay();
    }

    std::cout<<"Recon phase"<<std::endl;
    for (auto& regiment : frontlineRegiments_) {
        Nation& nation = nations_[regiment->ownerId_];
        //Check if the regiment is actually on the front-line
        std::vector<uint32_t> frontNeighbours;
        for (uint32_t tileId : tiles_[regiment->tileId_].neighbourIds_) {
            if (tiles_[tileId].ownerId_!=regiment->ownerId_) {
                frontNeighbours.emplace_back(tileId);
            }
        }
        if (!frontNeighbours.empty()) {
            std::cout<<"Regiment at "<<regiment->tileId_<<std::endl;
            //Randomly distribute the recon budget among our neighbours, weighted by 1 minus their recon
            int32_t budget = nation.getBattleStats().dailyReconPoints_;
            std::vector<float> weights(frontNeighbours.size());
            std::vector<uint32_t> points;
            for (uint32_t i = 0; i < frontNeighbours.size(); i++) {
                weights[i]=1.f-nation.getRecon(frontNeighbours[i]);
            }
            //This is the multinomial distribution
            for (uint32_t i = 0; i < frontNeighbours.size(); i++) {
                float totalWeight = 0.0f;
                for (uint32_t j = i; j < frontNeighbours.size(); j++) {
                    totalWeight += weights[j];
                }
                float w = safeDiv(weights[i],totalWeight);
                int32_t p = safeBinomial(budget,w,rng_);
                std::cout<<'\t'<<frontNeighbours[i]<<':'<<p<<std::endl;
                points.push_back(p);
                budget -= p;
            }
        }
    }
}



void Scene::render(SDL_Renderer *renderer, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis) const {
    for (const auto &tile : tiles_) {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        std::vector<SDL_FPoint> hex {
            SDL_FPoint(tile.x_+sin60_*tile.radius_,tile.y_-cos60_*tile.radius_),
            SDL_FPoint(tile.x_+sin60_*tile.radius_,tile.y_+cos60_*tile.radius_),
            SDL_FPoint(tile.x_,tile.y_+tile.radius_),
            SDL_FPoint(tile.x_-sin60_*tile.radius_,tile.y_+cos60_*tile.radius_),
            SDL_FPoint(tile.x_-sin60_*tile.radius_,tile.y_-cos60_*tile.radius_),
            SDL_FPoint(tile.x_,tile.y_-tile.radius_),
            SDL_FPoint(tile.x_+sin60_*tile.radius_,tile.y_-cos60_*tile.radius_)
        };
        SDL_RenderDrawLinesF(renderer,hex.data(),7);
        const auto& nation = nations_[tile.ownerId_];
        SDL_SetRenderDrawColor(renderer,nation.getRed(),nation.getGreen(),nation.getBlue(),255);
        hex=std::vector<SDL_FPoint> {
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.95f,tile.y_-cos60_*tile.radius_*0.95f),
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.95f,tile.y_+cos60_*tile.radius_*0.95f),
            SDL_FPoint(tile.x_,tile.y_+tile.radius_*0.95f),
            SDL_FPoint(tile.x_-sin60_*tile.radius_*0.95f,tile.y_+cos60_*tile.radius_*0.95f),
            SDL_FPoint(tile.x_-sin60_*tile.radius_*0.95f,tile.y_-cos60_*tile.radius_*0.95f),
            SDL_FPoint(tile.x_,tile.y_-tile.radius_*0.95f),
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.95f,tile.y_-cos60_*tile.radius_*0.95f)
        };
        SDL_RenderDrawLinesF(renderer,hex.data(),7);
        hex=std::vector<SDL_FPoint> {
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.9f,tile.y_-cos60_*tile.radius_*0.9f),
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.9f,tile.y_+cos60_*tile.radius_*0.9f),
            SDL_FPoint(tile.x_,tile.y_+tile.radius_*0.9f),
            SDL_FPoint(tile.x_-sin60_*tile.radius_*0.9f,tile.y_+cos60_*tile.radius_*0.9f),
            SDL_FPoint(tile.x_-sin60_*tile.radius_*0.9f,tile.y_-cos60_*tile.radius_*0.9f),
            SDL_FPoint(tile.x_,tile.y_-tile.radius_*0.9f),
            SDL_FPoint(tile.x_+sin60_*tile.radius_*0.9f,tile.y_-cos60_*tile.radius_*0.9f)
        };
        SDL_RenderDrawLinesF(renderer,hex.data(),7);
        if (!tile.frontlineRegiments_.empty()) {
            regimentTexture_->render(tile.x_-regimentTexture_->getWidth(),tile.y_-regimentTexture_->getHeight(),nation.getRed(),nation.getGreen(),nation.getBlue(),renderer);
            smallNumberRenderer_.render(tile.frontlineRegiments_.size(),tile.x_,tile.y_-regimentTexture_->getHeight(),nation.getRed(),nation.getGreen(),nation.getBlue(),renderer,1.0,false,false,false,true);
        }
    }

}