#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"
#include"RUI_DessertManager.h"
#include"RUI_ProductManager.h"

class Cabinet
{
    public:
    Cabinet() = default;
    ~Cabinet() = default;

    void InitCabinet(int id,int did)
    {
        CabinetID = id;
        x = (CabinetID / 12) * 200 + 100 * (CabinetID % 2) + 20;
        y = (CabinetID / 2 % 6) * 40 + 200; 
        DessertID = did;
        // DessertID = rand() % 4;
    }

    int GetX()
    {
        return x;
    }

    int GetY()
    {
        return y;
    }

    int GetDessertID()
    {
        return DessertID;
    }

    void SetDessertID(int id)
    {
        DessertID = id;
    }

    bool isClicked(int mx, int my)
    {
        if(mx >= x && mx <= x + 64)
        {
            if(my >= y && my <= y + 64)
            {
                return true;
            }
        }
        return false;
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

class CabinetFrame
{
    public:
    CabinetFrame() = default;
    ~CabinetFrame() = default;

    void SetCabinetID(int x)
    {
        IntCabinetID = x;
    }

    int GetCabinetID()
    {
        return IntCabinetID;
    }

    void InitFrame()
    {
        CabinetFrameTexture = ResourceManager::instance()->FindTexture("saving");
        QuitIcon = ResourceManager::instance()->FindTexture("quiticon");
        Rect = {150,50,500,500};
        QuitRect = {580,100,32,32};
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        color = {10,10,10,255};
        IntCabinetID = -1;
    }

    void quit()
    {
        SDL_FreeSurface(TitleSurface);
        SDL_FreeSurface(DessertSurface);
        TitleSurface = nullptr;
        DessertSurface = nullptr;
        SDL_DestroyTexture(TitleTexture);
        SDL_DestroyTexture(DessertNameTexture);
        TitleTexture = nullptr;
        DessertNameTexture = nullptr;
    }

    void onRender(SDL_Renderer* Renderer, Cabinet& cab, ProductManager Manager)
    {
        SDL_RenderCopy(Renderer, CabinetFrameTexture, nullptr, &Rect);
        SDL_RenderCopy(Renderer, QuitIcon, nullptr, &QuitRect);
        DessertName = Manager.GetProductName(cab.GetDessertID()) + " " + std::to_string(Manager.GetProductPrice(cab.GetDessertID())) + "元";
        CabinetID = "第" + std::to_string(IntCabinetID + 1) + "个面包柜";
        //SDL_Log("%d",IntCabinetID);

        if(TitleSurface == nullptr)
            TitleSurface = TTF_RenderUTF8_Blended(TextFont, CabinetID.c_str(), color);
        if(DessertSurface == nullptr)
            DessertSurface = TTF_RenderUTF8_Blended(TextFont, DessertName.c_str(), color);

        if(TitleTexture == nullptr)
            TitleTexture = SDL_CreateTextureFromSurface(Renderer,TitleSurface);
        if(DessertNameTexture == nullptr)
            DessertNameTexture = SDL_CreateTextureFromSurface(Renderer,DessertSurface);

        int tw = TitleSurface->w; int th = TitleSurface->h;
        int dw = DessertSurface->w; int dh = DessertSurface->h;

        TitleRect = {200,100,tw,th};
        DessertRect = {200, 400, dw, dh};

        SDL_RenderCopy(Renderer, TitleTexture, nullptr, &TitleRect);
        SDL_RenderCopy(Renderer, DessertNameTexture, nullptr, &DessertRect);
        Manager.onRender(Renderer,cab.GetDessertID());
    }

    private:
    TTF_Font* TextFont = nullptr;
    SDL_Texture* CabinetFrameTexture = nullptr;
    SDL_Texture* TitleTexture = nullptr;
    SDL_Texture* DessertNameTexture = nullptr;
    SDL_Texture* QuitIcon = nullptr;
    SDL_Rect Rect;
    SDL_Rect QuitRect;
    SDL_Rect TitleRect;
    SDL_Rect DessertRect;
    SDL_Surface* TitleSurface = nullptr;
    SDL_Surface* DessertSurface = nullptr;
    std::string DessertName;
    std::string CabinetID;
    SDL_Color color;
    int IntCabinetID;
};


class PayCharm
{
    public:
    PayCharm() = default;
    ~PayCharm() = default;

    void Init()
    {
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",24);
        Money = 0;
    }

    void SetPrice(int i)
    {
        Money = i;
        std::string price = std::to_string(Money) + "元";
        color = {10,10,10,255};
        PriceSurface = TTF_RenderUTF8_Blended(TextFont, price.c_str(), color);
        w = PriceSurface->w; h = PriceSurface->h;
        x = 350; y = 120;
        Rect = {x,y,w,h};
        StartTime = 0;
        StopTime = 0;
    }

    int GetMoney()
    {
        return Money;
    }

    void SetStartTime(int CurrentTime)
    {
        StartTime = CurrentTime;
    }

    void SetStopTime(int CurrentTime)
    {
        StopTime = CurrentTime;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if( StopTime - StartTime < 1000)
        {
            if(PriceTexture == nullptr)
            {
                if(PriceSurface == nullptr)
                    SDL_Log("nullptr");
                PriceTexture = SDL_CreateTextureFromSurface(Renderer,PriceSurface);
            }
            SDL_RenderCopy(Renderer, PriceTexture, nullptr, &Rect);
        }
    }

    private:
    int Money;
    int StartTime;
    int StopTime;
    int x,y;
    int w,h;
    TTF_Font* TextFont;
    SDL_Rect Rect;
    SDL_Surface* PriceSurface;
    SDL_Texture* PriceTexture = nullptr;
    SDL_Color color;
};