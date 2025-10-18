#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

class ProducingProduct
{
    public:
    ProducingProduct() = default;
    ~ProducingProduct() = default;

    void Init()
    {
        PlateSize = -1;
        BaseMaterialID = -1;
        for(int i = 0; i < 3; i++)
        {
            CreateMaterialID[i] = -1;
            DecorationID[i] = -1;
        }
    }

    void SetPlateSize(int size)
    {
        PlateSize = size;
    }

    int GetPlateSize()
    {
        return PlateSize;
    }

    private:
    int PlateSize;
    int BaseMaterialID;
    int CreateMaterialID[3];
    int DecorationID[3];
};