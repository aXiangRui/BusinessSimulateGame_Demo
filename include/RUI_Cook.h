#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"
#include"RUI_Product.h"
#include"RUI_Cabinet.h"

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
        Address = "default";
        x = 200;
        y = 50;
        CookRect = {200,50,64,64};
        toward = 1;
    }

    void onUpdate(int CurrentTime, std::vector<SmallCake>& cakes, std::vector<Cabinet> Cabinets, int ClockTime)
    {
        if(ClockTime >= 6 && ClockTime <= 22)
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
                    for(int i = 0; i < 12; i++)
                    {
                        SmallCake a;
                        int num = cakes.size();
                        a.init(num, rand() % Cabinets.size());
                        cakes.push_back(a);
                        // productID.push_back(rand() % Cabinets.size());
                        SDL_Log("新增%d号面包柜甜点",cakes.back().GetID()+1);
                    }
                }
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

    void SetPosition(int mx, int my)
    {
        x = mx; y = my;
    }
    void SetAddress(std::string add)
    {
        Address = add;
        CookTexture = ResourceManager::instance()->FindTexture(Address.c_str());
    }
    std::string GetAddress()
    {
        return Address;
    }

    int GetX()
    {
        return x;
    }

    int GetY()
    {
        return y;
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
    std::string Address;
    SDL_Texture* CookTexture;
    SDL_Rect CookRect;
};