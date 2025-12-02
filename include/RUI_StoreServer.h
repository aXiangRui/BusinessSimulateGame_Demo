#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "RUI_ResourceManager.h"

class StoreServer
{
    public:
        StoreServer() = default;
        ~StoreServer() = default;

    void init()
    {
        x = 0;
        y = 200;
        toward = 0;
        ServerTexture = ResourceManager::instance()->FindTexture("default");
        ServerRect = { x, y, 64, 64};
    }

    void onRender(SDL_Renderer* Renderer)
    {
        SDL_RenderCopy(Renderer, ServerTexture, nullptr, &ServerRect);
    }

    private:
    int x,y;
    int toward;
    SDL_Texture* ServerTexture;
    SDL_Rect ServerRect;
};