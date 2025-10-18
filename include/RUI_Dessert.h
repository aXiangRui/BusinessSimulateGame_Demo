#pragma once

#include<SDL2/SDL.h>
#include<string>
#include"RUI_ResourceManager.h"

class Dessert
{
    public:
       Dessert() = default;
       ~Dessert() = default;
    SDL_Texture* DessertTexture;

    void InitLevel(int dessertid, int SweetNumber, int FullNumber, int TasteNumber, std::string name,std::string path)
    {
        DessertID = dessertid;
        SweetLevel = SweetNumber;
        FullLevel = FullNumber;
        TasteLevel = TasteNumber;
        FilePath = path;
        Name = name;
        DessertTexture = ResourceManager::instance()->FindTexture(path.c_str());
        price = SweetLevel/10 * 3 + FullLevel/10 * 1 + TasteLevel/10 * 2;
        lastTime = 0;
        isClicked = 0;

        x = 200;
        y = 200;
        w = 256;
        h = 256;
    }

    int GetDessertID()
    {
        return DessertID;
    }

    std::string GetDessertName()
    {
        return Name;
    }

    std::string GetDessertPath()
    {
        return FilePath;
    }

    int GetDessertPrice()
    {
        return price;
    }

    void AddSweetNumber(int SweetNumber)
    {
        SweetLevel = SweetLevel + SweetNumber;
    }

    void AddFullNumber(int FullNumber)
    {
        FullLevel = FullLevel + FullNumber;
    }

    void AddTasteNumber(int TasteNumber)
    {
        TasteLevel = TasteLevel + TasteNumber;
    }

    int GetSweetNumber()
    {
        return SweetLevel;
    }

    int GetFullNumber()
    {
        return FullLevel;
    }

    int GetTasteNumber()
    {
        return TasteLevel;
    }

    void SetName(std::string name)
    {
        Name = name;
    }

    std::string GetName()
    {
        return Name;
    }

    void SetPosition(int mx, int my)
    {
        x = mx;
        y = my;
    }

    void SetWidth(int mw, int mh)
    {
        w = mw;
        h = mh;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        DessertTexture = ResourceManager::instance()->FindTexture(FilePath.c_str());
        Rect = {x,y,w,h};
        SDL_RenderCopy(Renderer,DessertTexture,nullptr,&Rect);
    }

    void onRender(SDL_Renderer* Renderer,SDL_Rect newRect)
    {
        DessertTexture = ResourceManager::instance()->FindTexture(FilePath.c_str());
        SDL_RenderCopy(Renderer,DessertTexture,nullptr,&newRect);
    }

    void onRender(SDL_Renderer* Renderer,int mx,int my)
    {
        DessertTexture = ResourceManager::instance()->FindTexture(FilePath.c_str());
        Rect = {mx,my,w,h};
        SDL_RenderCopy(Renderer,DessertTexture,nullptr,&Rect);
    }

    void SetIsClicked(bool x)
    {
        isClicked = x;
    }

    void moveUpdate(int CurrentTime)
    {
        if(true)
        {
            if(CurrentTime - lastTime >= 16 && y <= 160)
            {
                y = y + 5;
                Rect = {x,y,w,h};
                lastTime = CurrentTime;
            }
        }
    }

    private:
    int SweetLevel;
    int FullLevel;
    int TasteLevel;
    int DessertID;
    int price;
    std::string Name;
    std::string FilePath;
    int x,y;
    int w,h;
    int lastTime;
    bool isClicked;
    SDL_Rect Rect;
};