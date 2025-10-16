#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<windows.h>
#include"RUI_ResourceManager.h"

void LoadingSc(SDL_Renderer* Renderer)
{
    TTF_Font* Textfont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
    SDL_Color color = {15,15,15,255};
    SDL_Surface* image = TTF_RenderUTF8_Blended(Textfont,"正在加载中",color);
    if(image)
    {    
        int w = image->w;
        int h = image->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);
        SDL_FreeSurface(image);
        SDL_Rect Rect = {(800-w)/2,(600-h)/2,w,h};
        
        SDL_SetRenderDrawColor(Renderer,230,200,200,255);


        SDL_RenderClear(Renderer); 
        SDL_RenderCopy(Renderer,texture,nullptr,&Rect);  
        SDL_RenderPresent(Renderer);
    }
  
    ResourceManager::instance()->load(Renderer);
}