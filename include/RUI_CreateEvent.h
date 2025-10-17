#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_ChooseFrame.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<vector>

class CreateRUIEvent
{
    public:
    CreateRUIEvent() = default;
    ~CreateRUIEvent() = default;

    std::vector<ChooseFrame> SizeFrames;
    std::vector<ChooseFrame> BaseFrames;
    std::vector<ChooseFrame> DecorationFrames;

    void load()
    {
        CStage = Stage::size;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");
    }

    void ChooseSize(std::vector<int>& Size)
    {
        DessertSize = Size;
        for(int i = 0; i < DessertSize.size(); i++)
        {
            ChooseFrame AddFrame;
            std::string a;
            switch(i)
            {
                case 0: a = "小";break;
                case 1: a = "中";break;
                case 2: a = "大";break;
                default:break;
            }
            AddFrame.InitChooseFrame(500, 10 + i * 50, i, a.c_str());
            SizeFrames.push_back(AddFrame);
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
    switch(CStage)
    {
        case Stage::size:
        {
            for(int i = 0; i < SizeFrames.size(); i++)
            {
                SizeFrames[i].onRender(Renderer);
            }
            break;
        }
    }
    }

    void input(const SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_MOUSEMOTION:
            {
                int mx = event.motion.x; int my = event.motion.y;
                switch (CStage)
                {
                    case Stage::size:{SizeChoose(mx,my);break;}
                    default:break;
                }
            }
        }
    }

    void SizeChoose(int mx, int my)
    {
        for(int i = 0; i < SizeFrames.size(); i++)
        {
            if(SizeFrames[i].isHovered(mx,my))
            {}
        }
    }

    void quit()
    {
        DessertSize.clear();
        SizeFrames.clear();
    }

    void SetStage(int i)
    {
        switch(i)
        {
            case 1:{CStage = Stage::size;     break;}
            case 2:{CStage = Stage::base;     break;}
            case 3:{CStage = Stage::create;   break;}
            case 4:{CStage = Stage::decorate; break;}
            default:break;
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