#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"

class Cook
{
    public:
    Cook() = default;
    ~Cook() = default;

    void Init()
    {
        Choose = 1;
        isCooking = 0;
        Send = 0;
        Speed = 10;
        WalkSpeed = 2;
        ChooseTime = 0;
        CookingTime = 0;
        SendTime = 0;
        CookTexture = ResourceManager::instance()->FindTexture("default");
        x = 200;
        y = 50;
        CookRect = {200,50,64,64};
        toward = 1;
    }

    void onUpdate(int CurrentTime)
    {
        if(Choose == 1)
        {
            if(x >= 10)
            {
                x = x - WalkSpeed;
                toward = 0;
            }
            else if( y >= 10)
            {
                y = y - WalkSpeed;
            }
            else if(ChooseTime == 0)
            {
                ChooseTime = CurrentTime;
            }
            else if( CurrentTime - ChooseTime >= 2000)
            {
                Choose = 0;
                isCooking = 1;
                ChooseTime = 0;
            }
        }
        if(isCooking == 1)
        {
            if( x <= 350)
            {
                toward = 1;
                x = x + WalkSpeed;
            }
            else if( y <= 80)
            {
                y = y + WalkSpeed;
            }
            else if( CookingTime == 0)
            {
                CookingTime = CurrentTime;
            }
            else if( CurrentTime - CookingTime >= 2000)
            {
                isCooking = 0;
                Send = 1;
                CookingTime = 0;
            }
        }
        if(Send == 1)
        {
            if(x >= 100)
            {
                toward = 0;
                x = x - WalkSpeed;
            }
            else if(y <= 80)
            {
                y = y + WalkSpeed;
            }
            else if(SendTime == 0)
            {
                SendTime = CurrentTime;
            }
            else if(CurrentTime - SendTime >= 2000)
            {
                Send = 0;
                Choose = 1;
                SendTime = 0;
            }
        }
    }

    void onRender(SDL_Renderer*  Renderer)
    {
        CookRect = {x, y, 64, 64};
        if(toward == 0)
            SDL_RenderCopy(Renderer, CookTexture, nullptr, &CookRect);
        else
            SDL_RenderCopyEx(Renderer, CookTexture, nullptr, &CookRect, 0, 0, SDL_FLIP_HORIZONTAL);
    }

    private:
    bool Choose;
    bool isCooking;
    bool Send;
    int ChooseTime;
    int CookingTime;
    int SendTime;
    int Speed;
    int WalkSpeed;
    bool toward;
    int x,y;
    SDL_Texture* CookTexture;
    SDL_Rect CookRect;
};