#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<string>

class Clock
{
    public:
        Clock() = default;
        ~Clock() = default;

    int ClockTime;
    TTF_Font* TimeFont;
    SDL_Color TextColor = {10, 10, 10, 255};

    void SetClockTime(int time)
    {
        ClockTime = time;
    }

    void SetStartTime(int i)
    {
        ClockTime = i;
    }

    void UpdateTime()
    {
        ClockTime = ClockTime + 1;
    }

    int ReturnHour()
    {
        return ClockTime % 24;
    }

    int ReturnDay()
    {
        return (ClockTime / 24) + 1;
    }

    int ReturnAllHour()
    {
        return ClockTime;
    }

    void RenderHour(SDL_Renderer* Renderer)
    {
        if(!TimeFont)
            TimeFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        std::string j;
        j = "第" + std::to_string(ReturnDay()) + "天";
        if(ReturnHour() >= 10)
            j = j + std::to_string(ReturnHour()) + ":00" ;
        else
        {
            j = j + " " + std::to_string(ReturnHour()) + ":00" ;
        }             
        SDL_Surface* image = TTF_RenderUTF8_Blended(TimeFont,j.c_str(),TextColor);
        int w = image->w;
        int h = image->h;
        SDL_Rect Rect = {10, 10, w, h};
        SDL_Rect Rect01 = {5,5,w+10,h+10};
        SDL_SetRenderDrawColor(Renderer,230,230,230,255);
        SDL_RenderFillRect(Renderer,&Rect01);
        SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer,image);
        SDL_FreeSurface(image);
        SDL_RenderCopy(Renderer, Texture, nullptr, &Rect);
    }

};