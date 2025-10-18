#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include"RUI_MaterialManager.h"

class ProducingProduct
{
    public:
    ProducingProduct() = default;
    ~ProducingProduct() = default;

    void Init(MaterialManager materialManager)
    {
        PlateSize = -1;
        BaseMaterialID = -1;
        for(int i = 0; i < 3; i++)
        {
            CreateMaterialID[i] = -1;
            DecorationID[i] = -1;
        }
        CreateNumber = 3;
        DecorateNumber = 3;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        color = {10,10,10,255};
        manager = materialManager;
    }

    void SetPlateSize(int size)
    {
        PlateSize = size;
    }

    int GetPlateSize()
    {
        return PlateSize;
    }

    void SetBaseID(int id)
    {
        BaseMaterialID = id;
    }

    int GetBaseID()
    {
        return BaseMaterialID;
    }

    int GetLastCreateNumbers()
    {
        return 3 - CreateNumber;
    }

    void RenderCreateNumbers(SDL_Renderer* Renderer)
    {
        std::string Cnumber = "剩余" + std::to_string(CreateNumber) + "次";
        CNumberSurface = TTF_RenderUTF8_Blended(TextFont,Cnumber.c_str(),color);
        int mw = CNumberSurface->w; int mh = CNumberSurface->h;
        CRect = {10,10,mw,mh};
        CNumberTexture = SDL_CreateTextureFromSurface(Renderer,CNumberSurface);
        SDL_RenderCopy(Renderer, CNumberTexture, nullptr, &CRect);
    }

    void moveUpdate(int CurrentTime)
    {
        for(int i = 0 ; i < 3; i++)
        {
            if(CreateMaterialID[i] != -1)
            {
                create[i].moveUpdate(CurrentTime,i);
            }
        }
    }

    void RenderCreateMaterial(SDL_Renderer* Renderer)
    {
        for(int i = 0; i < 3; i++)
        {
            if(CreateMaterialID[i] != -1)
            {
                create[i].onRender(Renderer);
            }
        }
    }

    void SetCreateCase(int i, int j)
    {
        CreateMaterialID[i] = j;
        create[i] = manager.DecorationMaterial[j];
        CreateNumber--;
    }

    private:
    int PlateSize;
    int BaseMaterialID;
    int CreateMaterialID[3];
    int DecorationID[3];
    int CreateNumber;
    int DecorateNumber;
    TTF_Font* TextFont;
    SDL_Surface* CNumberSurface = nullptr;
    SDL_Surface* DNumberSurface = nullptr;
    SDL_Rect CRect;
    SDL_Rect DRect;
    SDL_Texture* CNumberTexture = nullptr;
    SDL_Texture* DnumberTexture = nullptr;
    SDL_Color color;
    MaterialManager manager;
    Material create[3];
};

class Cloth
{
    public:
    Cloth() = default;
    ~Cloth() = default;

    void init()
    {
        ClothTexture = ResourceManager::instance()->FindTexture("cloth");
        LastTime = 0;
        DelayTime = 0;
        x = 0; y = -600;
        Rect = {x,y,600,600};
        a = 1;
    }

    void SetWhetherRender(bool a)
    {
        whetherRender = 1;
    }

    void Appear(int CurrentTime)
    {
        if(a)
        {
            if(whetherRender == 1)
            {
                if(CurrentTime - LastTime >= 16 && y <= -50)
                {
                    y = y + 10;
                    Rect = {x,y,500,500};
                    LastTime = CurrentTime;
                }
            }
            if(y > -50)
            {
                DelayTime = CurrentTime;
                a = 0;
            }
        }
        
    }

    void Quit(int CurrentTime)
    {
        if(DelayTime != 0)
        {
            SDL_Log("%d",DelayTime);
            if(CurrentTime - DelayTime > 1000)
            {
                if(CurrentTime - LastTime >= 16 && y >= -600)
                {
                    y = y - 10;
                    Rect = {x,y,500,500};
                }
            }
            if(y < -600)
            {
                whetherRender = 0;
            }
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(whetherRender)
            SDL_RenderCopy(Renderer,ClothTexture, nullptr, &Rect);
    }

    private:
    SDL_Texture* ClothTexture;
    SDL_Rect Rect;
    int x,y;
    bool whetherRender;
    int LastTime;
    int DelayTime;
    int a;
};