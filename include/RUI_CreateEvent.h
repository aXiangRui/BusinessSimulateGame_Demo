#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<vector>

class CreateRUIEvent
{
    public:
    CreateRUIEvent() = default;
    ~CreateRUIEvent() = default;

    // void onUpdate(std::vector<int>& Size)
    // {

    // }

    void load()
    {
        CStage = Stage::size;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");
    }

    void ChooseSize(std::vector<int>& Size)
    {
        DessertSize = Size;
    }

    void onRender(SDL_Renderer* Renderer)
    {
    switch(CStage)
    {
        case Stage::size:
        {
            for(int i = 0; i < DessertSize.size(); i++)
            {
                SDL_Rect Rect = {400, 50 + i * 30 , 320, 64};
                SDL_RenderCopy(Renderer, ChooseFrameTexture, nullptr, &Rect);
            }
            break;
        }
    }
    }

    void SetStage(int i)
    {
        switch(i)
        {
            case 1:
            {
                CStage = Stage::size;
                break;
            }
            case 2:
            {
                CStage = Stage::base;
                break;
            }
            case 3:
            {
                CStage = Stage::create;
                break;
            }
            case 4:
            {
                CStage = Stage::decorate;
                break;
            }
            default:
            break;
        }
    }

    enum class Stage
    {
        size,
        base,
        create,
        decorate
    };

    Stage CStage;
    std::vector<int> DessertSize;
    TTF_Font* TextFont;
    SDL_Texture* ChooseFrameTexture;
};