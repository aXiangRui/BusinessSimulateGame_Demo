#pragma once

#include"RUI_Button.h"

class MenuButton: public Button
{
    public:

    MenuButton() = default;
    ~MenuButton() = default;

    MenuButton(int mx, int my, int mw, int mh, std::string t, int ID)
    {
        x = mx;
        y = my;
        w = mw;
        h = mh;
        text = t;
        id = ID;
    }

    void loadTexture(int id)
    {

    }
    void ClickApplication()
    {
        switch(id)
        {
        }
    }
    void ButtonRender(SDL_Renderer* Renderer)
    {
        SDL_Surface* image = IMG_Load("./resources/texture/button/buttonDemo.png");
        SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);
        SDL_Rect Rect = {x,y,w,h};
        SDL_RenderCopy(Renderer,texture,nullptr,&Rect);
    }

};