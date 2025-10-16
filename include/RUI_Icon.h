#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<string>
#include"RUI_ResourceManager.h"

class RUI_Icon
{
    public:
    RUI_Icon() = default;
    ~RUI_Icon() = default;
    
    // virtual void IconHoverRender(SDL_Renderer* Renderer,std::string address);
    // virtual void IconClickRender(SDL_Renderer* Renderer,std::string address);

    void InitIcon(int mx, int my, int mw, int mh, int i, std::string add)
    {
        x = mx;
        y = my;
        w = mw;
        h = mh;
        Rect = {x,y,w,h};
        id = i;
        address = add;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        IconTexture = ResourceManager::instance()->FindTexture(address.c_str());
        SDL_RenderCopy(Renderer,IconTexture,nullptr,&Rect);
    }

    bool isHovered(int mx, int my)
    {
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    bool isClicked(int mx, int my)
    {
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    private:
    int x,y,w,h;
    SDL_Rect Rect;
    int id;
    std::string address;
    SDL_Texture* IconTexture;
};