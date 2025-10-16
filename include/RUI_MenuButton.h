#pragma once

#include"RUI_Button.h"
#include"RUI_ResourceManager.h"

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
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
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
        SDL_Texture* texture = ResourceManager::instance()->FindTexture("buttonDemo");        
        ButtonRect = {x,y,w,h};
        if(texture)
            SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);

        if(!TextFont)
        {
            SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        }
        else
        {
            setTextColor(0,0,0,255);
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
            if(textSurface)
            {
                TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                TextRect = {x+(w-tw)/2,y+(h-th)/2,tw,th};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
                if(!TextTexture)
                    SDL_Log("CreateTextTexture failed: %s", SDL_GetError());
            }
            else
            {
                SDL_Log("TTF_RenderUTF8_Blended failed: %s", TTF_GetError());
            }
        }
    }

    void HoveredButtonRender(SDL_Renderer* Renderer)
    {
            SDL_Texture* texture = ResourceManager::instance()->FindTexture("buttonDemo_hovered");
            ButtonRect = {x,y,w,h};
            if(texture)
                SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);
            if(!TextFont)
            {
                SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
            }
            else
            {
                setTextColor(0,0,0,255);
                SDL_Surface* textSurface = nullptr;
                if(TextFont != nullptr)
                    textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
                if(textSurface)
                {
                    TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
                    int tw = textSurface->w;
                    int th = textSurface->h;
                    TextRect = {x+(w-tw)/2,y+(h-th)/2,tw,th};
                    SDL_FreeSurface(textSurface);
                    SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
                    if(!TextTexture)
                        SDL_Log("CreateTextTexture failed: %s", SDL_GetError());
                }
                else
                {
                    SDL_Log("TTF_RenderUTF8_Blended failed: %s", TTF_GetError());
                }
            }
    }

    void ClickedButtonRender(SDL_Renderer* Renderer)
    {
            SDL_Texture* texture = ResourceManager::instance()->FindTexture("buttonDemo_clicked");
            ButtonRect = {x,y,w,h};
            if(texture)
                SDL_RenderCopy(Renderer,texture,nullptr,&ButtonRect);
            else
            {
                SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
            }
  
            
                setTextColor(0,0,0,255);
                SDL_Surface* textSurface = TTF_RenderUTF8_Blended(TextFont,text.c_str(),TextColor);
                if(textSurface)
                {
                    TextTexture = SDL_CreateTextureFromSurface(Renderer,textSurface);
                    int tw = textSurface->w;
                    int th = textSurface->h;
                    TextRect = {x+(w-tw)/2,y+(h-th)/2,tw,th};
                    SDL_FreeSurface(textSurface);
                    SDL_RenderCopy(Renderer,TextTexture,nullptr,&TextRect);
                    if(!TextTexture)
                        SDL_Log("CreateTextTexture failed: %s", SDL_GetError());
                }
                else
                {
                    SDL_Log("TTF_RenderUTF8_Blended failed: %s", TTF_GetError());
                }
            
    }      
};