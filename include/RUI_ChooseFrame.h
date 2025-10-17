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
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",24);
    }

    bool isHovered(int mx, int my)
    {
        if(mx >= x && mx <= x + 320)
        {
            if(my >= y && my <= y + 64)
            {
                ishovered = 1;
                return true;
            }
        }
        return false;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(ishovered == 0)
        {
            SDL_RenderCopy(Renderer, ChooseFrameTexture, nullptr, &Rect);
        }
        else
        {
            SDL_RenderCopy(Renderer, ChooseHoveredFrameTexture, nullptr, &Rect);
        }
        SDL_Color color = {10,10,10,255};
        SDL_Surface* image = TTF_RenderUTF8_Blended(TextFont, Text.c_str(), color);
        TextTexture = SDL_CreateTextureFromSurface(Renderer,image);
        if(image)
        {
            int mw = image->w; int mh = image->h;  
            TextRect = { x + 20, y + 20, mw, mh};
            SDL_RenderCopy(Renderer, TextTexture, nullptr, &TextRect);
            SDL_FreeSurface(image);
        }   
        // SDL_DestroyTexture(TextTexture);
    }

    private:
    int Optionid;
    int x,y;
    SDL_Texture* ChooseFrameTexture;
    SDL_Texture* ChooseHoveredFrameTexture;
    SDL_Texture* TextTexture;
    SDL_Rect Rect;
    SDL_Rect TextRect;
    TTF_Font* TextFont;
    std::string Text;
    bool ishovered;
};