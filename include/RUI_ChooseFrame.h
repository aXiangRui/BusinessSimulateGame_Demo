#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include"RUI_ResourceManager.h"

class ChooseFrame
{
    public:
   ChooseFrame() = default;
    ~ChooseFrame() = default;

    void InitChooseFrame(int mx,int my, int id, std::string t)
    {
        x = mx;
        y = my;
        Optionid = id;
        ishovered = 0;
        Text = t;
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");
        ChooseHoveredFrameTexture = ResourceManager::instance()->FindTexture("chooseframe_hovered");
        Rect = {x,y,320,64};
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",32);
        
        SDL_Color color = {10,10,10,255};
        image = TTF_RenderUTF8_Blended(TextFont, Text.c_str(), color);
        //SDL_Log("%s",Text.c_str());

    }

    bool isHovered(int mx, int my)
    {
        bool hovered = (mx >= x && mx <= x + 320 && my >= y && my <= y + 64);
        ishovered = hovered ? 1 : 0;
        return hovered;
    }

    void onRender(SDL_Renderer* Renderer)
    {   
        
        SDL_RenderCopy(Renderer, ChooseFrameTexture, nullptr, &Rect);
        // if(image)
        // {
        int mw = image->w; int mh = image->h;  
        TextRect = { x + 20, y + 15, mw, mh};
        if(TextTexture == nullptr)
            TextTexture = SDL_CreateTextureFromSurface(Renderer,image); 
        //}  
        SDL_RenderCopy(Renderer, TextTexture, nullptr, &TextRect);
        // SDL_DestroyTexture(TextTexture);
        // TextTexture = nullptr;
    }

    void onHoverRender(SDL_Renderer* Renderer)
    {
        SDL_RenderCopy(Renderer, ChooseHoveredFrameTexture, nullptr, &Rect);
        if(image)
        {
            int mw = image->w; int mh = image->h;  
            TextRect = { x + 20, y + 15, mw, mh};
            if(TextTexture == nullptr)
                TextTexture = SDL_CreateTextureFromSurface(Renderer,image); 
        }  
        SDL_RenderCopy(Renderer, TextTexture, nullptr, &TextRect);
    }

    bool GetIsHovered()
    {
        return ishovered;
    }

    private:
    int Optionid;
    int x,y;
    SDL_Texture* ChooseFrameTexture;
    SDL_Texture* ChooseHoveredFrameTexture;
    SDL_Texture* TextTexture = nullptr;
    SDL_Surface* image = nullptr;
    SDL_Rect Rect;
    SDL_Rect TextRect;
    TTF_Font* TextFont = nullptr;
    std::string Text;
    bool ishovered;
};