#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"

class Chair
{
    public:
    Chair() = default;
    ~Chair() = default;

    void InitChair(int id)
    {
        ChairID = id;
        x = (ChairID / 8) * 200 + 450 + 80 * (ChairID % 2);
        y = (ChairID / 2 % 4) * 100; 
        if(ChairID % 2 == 0)
        {
            toward = 0;
        }
        else
        {
            toward = 1;
        }
        isUsing = 0;
    }

    void SetPosition(int mx, int my)
    {
        x = mx;
        y = my;
    }

    void SetUsing(bool x)
    {
        isUsing = x;
    }

    bool GetUsing()
    {
        return isUsing;
    }

    int GetChairID()
    {
        return ChairID;
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
        if(!ChairTexture)
            ChairTexture = ResourceManager::instance()->FindTexture("chair");
        SDL_Rect Rect = {x,y,64,64};
        if(toward == 0)
        {
            SDL_RenderCopyEx(Renderer,ChairTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
        }
        else
        {
            SDL_RenderCopy(Renderer,ChairTexture,nullptr,&Rect);
        }   
    }

    private:

    int ChairID;
    int x,y;
    bool isUsing;
    bool toward;
    SDL_Texture* ChairTexture = nullptr;

};