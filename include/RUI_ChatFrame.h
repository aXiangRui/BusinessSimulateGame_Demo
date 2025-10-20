#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include"RUI_ResourceManager.h"

class ChatFrame
{
    public:
    ChatFrame() = default;
    ~ChatFrame() = default;

    void init()
    {    
        FrameTexture = ResourceManager::instance()->FindTexture("saving");
        rect = {100,0,600,600};
    }

    void setID(int id)
    {
        ChatFrameID = id;
    }

    void setTitle(std::string title)
    {
        Title = title;
    }

    void setContent(std::string content)
    {
        Content = content;
    }

    int getID()
    {
        return ChatFrameID;
    }

    std::string getTitle()
    {
        return Title; 
    }

    std::string getContent()
    {
        return Content;
    }

    void RenderFrame(SDL_Renderer* Renderer)
    {      
        FrameTexture = ResourceManager::instance()->FindTexture("saving");
        rect = {100,0,600,600};
        SDL_RenderCopy(Renderer,FrameTexture,nullptr,&rect);
    }

    private:
    int ChatFrameID;
    std::string Title;
    std::string Content;
    SDL_Texture* FrameTexture;
    SDL_Rect rect;
};

class SummaryFrame
{
    public:
    SummaryFrame() = default;
    ~SummaryFrame() = default;

    void Init()
    {
        Font = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        BackTexture = ResourceManager::instance()->FindTexture("saving");
        BackRect = {100,0,600,600};
        color = {10,10,10,255};
        dw = 200; cw = 200; dh = 100; ch = 100;
    }

    void update(int TotalCustomers, int TotalDessert)
    {
        tCusomers = TotalCustomers;
        tDessert = TotalDessert;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        std::string CString = "今日共接待顾客" + std::to_string(tCusomers) + "名";
        std::string DString = "今日共卖出甜点" + std::to_string(tDessert) + "份";

        DMoneySurface = TTF_RenderUTF8_Blended(Font, DString.c_str(), color);
        CMoneySurface = TTF_RenderUTF8_Blended(Font, CString.c_str(), color);
        dw = DMoneySurface->w; dh = DMoneySurface->h;
        cw = CMoneySurface->w; ch = CMoneySurface->h;

        DMoneyTexture = SDL_CreateTextureFromSurface(Renderer, DMoneySurface);
        CMoneyTexture = SDL_CreateTextureFromSurface(Renderer, CMoneySurface);
        // SDL_Log("%d %d %d %d",dw,dh,cw,ch);

        DRect = {(800-dw)/2,200,dw,dh};
        CRect = {(800-cw)/2,400,cw,ch};

        SDL_RenderCopy(Renderer, BackTexture, nullptr, &BackRect);
        SDL_RenderCopy(Renderer, DMoneyTexture, nullptr, &DRect);
        SDL_RenderCopy(Renderer, CMoneyTexture, nullptr, &CRect);

    }
    private:
    TTF_Font* Font;
    SDL_Texture* BackTexture;
    SDL_Texture* DMoneyTexture;
    SDL_Texture* CMoneyTexture;
    SDL_Surface* DMoneySurface;
    SDL_Surface* CMoneySurface;
    SDL_Rect BackRect;
    SDL_Rect DRect;
    SDL_Rect CRect;
    SDL_Color color;
    int dw,dh,cw,ch;
    int tCusomers;
    int tDessert;
};