#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"

class Cabinet
{
    public:
    Cabinet() = default;
    ~Cabinet() = default;

    void InitCabinet(int id)
    {
        CabinetID = id;
        x = (CabinetID / 12) * 200 + 100 * (CabinetID % 2) + 20;
        y = (CabinetID / 2 % 6) * 40 + 200; 
    }

    int GetX()
    {
        return x;
    }

    int GetY()
    {
        return y;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(!CabinetTexture)
            CabinetTexture = ResourceManager::instance()->FindTexture("cabinet");
        SDL_Rect Rect = {x,y,64,64};
        SDL_RenderCopy(Renderer,CabinetTexture,nullptr,&Rect);
    }
    
    private:
    int CabinetID;
    int DessertID;
    int x,y;
    SDL_Texture* CabinetTexture = nullptr;
};