#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_Material.h"
#include"RUI_ResourceManager.h"

class DecorationMaterial : public Material
{
    public:
        DecorationMaterial() = default;
        ~DecorationMaterial() = default;

        void InitMaterial()
        {
            
        }

        void RenderMaterial(SDL_Renderer* Renderer)
        {
        }
};