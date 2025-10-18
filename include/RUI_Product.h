#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

class ProducingProduct
{
    public:
    ProducingProduct() = default;
    ~ProducingProduct() = default;

    void Init()
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

    void RenderCreateNumbers(SDL_Renderer* Renderer)
    {
        std::string Cnumber = "剩余" + std::to_string(CreateNumber) + "次";
        CNumberSurface = TTF_RenderUTF8_Blended(TextFont,Cnumber.c_str(),color);
        int mw = CNumberSurface->w; int mh = CNumberSurface->h;
        CRect = {10,10,mw,mh};
        CNumberTexture = SDL_CreateTextureFromSurface(Renderer,CNumberSurface);
        SDL_RenderCopy(Renderer, CNumberTexture, nullptr, &CRect);
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
};