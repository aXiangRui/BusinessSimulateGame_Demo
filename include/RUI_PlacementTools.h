#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "RUI_ResourceManager.h"
#include <iostream>
#include <string>

class BorderBox
{
    public:
    BorderBox() = default;
    ~BorderBox() = default;
    void InitBorderBox(int mx, int my, int mw, int mh ,std::string add)
    {
        borderBoxTexture = ResourceManager::instance()->FindTexture(add.c_str());
        borderBoxRect = {mx,my,mw,mh};
        isChoosing = false;
    }
    void onMotionInput(bool isFurniturePlacing, const int mx,const int my)
    {
        if( mx > 0 && mx < borderBoxRect.w && isFurniturePlacing)
        {
            isChoosing = true;
        }
        else
        {
            isChoosing = false;
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(isChoosing)
        {
            SDL_RenderCopy(Renderer,borderBoxTexture,nullptr,&borderBoxRect);
        }
    }

    void onUpdate()
    {

    }

    void onQuit()
    {

    }

    private:
    SDL_Texture* borderBoxTexture;
    SDL_Rect borderBoxRect;
    bool isChoosing;
};