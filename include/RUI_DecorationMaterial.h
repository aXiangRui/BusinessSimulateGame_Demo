#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_Material.h"

class DecorationMaterial : public Material
{
    public:
        DecorationMaterial() = default;
        ~DecorationMaterial() = default;

        void RenderMaterial(SDL_Renderer* Renderer)
        {
            SDL_Surface* image = IMG_Load(GetPath().c_str());
            MaterialTexture = SDL_CreateTextureFromSurface(Renderer,image);
            int mw = image->w;
            int mh = image->h;
            SDL_Rect Rect = {GetXPosition(), GetYPosition(), mw, mh};
            SDL_FreeSurface(image);
            SDL_RenderCopy(Renderer, MaterialTexture, nullptr, &Rect);
        }
};