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
        ButtonRect = {x,y,w,h};
        SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);

        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        setTextColor(0,0,0,255);
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
        int tw = textSurface->w;
        int th = textSurface->h;
        int tx = x + (w - tw) / 2;
        int ty = y + (h - th) / 2;
        TextRect = {tx,ty,tw,th};
        TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
    }

    void HoveredButtonRender(SDL_Renderer* Renderer)
    {
            SDL_Surface* image = IMG_Load("./resources/texture/button/buttonDemo_hovered.png");

            SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);        
            ButtonRect = {x,y,w,h};
            SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);

            TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            setTextColor(0,0,0,255);
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
            int tw = textSurface->w;
            int th = textSurface->h;
            int tx = x + (w - tw) / 2;
            int ty = y + (h - th) / 2;
            TextRect = {tx,ty,tw,th};
            TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
    }

    void ClickedButtonRender(SDL_Renderer* Renderer)
    {
            SDL_Surface* image = IMG_Load("./resources/texture/button/buttonDemo_clicked.png");
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);        
            ButtonRect = {x,y,w,h};
            SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);

            TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            setTextColor(0,0,0,255);
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
            int tw = textSurface->w;
            int th = textSurface->h;
            int tx = x + (w - tw) / 2;
            int ty = y + (h - th) / 2;
            TextRect = {tx,ty,tw,th};
            TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
    }
        

};