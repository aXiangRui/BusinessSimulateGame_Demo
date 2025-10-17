#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_BaseMaterial.h"
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
    std::vector<Plate> plates;

    void load()
    {
        CStage = Stage::size;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");
        for(int i = 0; i < 3; i++)
        {
            Plate a;
            a.InitPlate(140,-60,i);
            plates.push_back(a);
        }
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

    void update()
    {
        CurrentTime = SDL_GetTicks();
    }

    void onRender(SDL_Renderer* Renderer)
    {
    switch(CStage)
    {
        case Stage::size:
        {
            for(int i = 0; i < SizeFrames.size(); i++)
            {
                if(SizeFrames[i].GetIsHovered())
                {
                    SizeFrames[i].onHoverRender(Renderer);
                    plates[i].AnimationRender(Renderer,CurrentTime);
                }
                else
                {
                    SizeFrames[i].onRender(Renderer);
                }              
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
                    case Stage::size:
                    {
                        for(int i = 0; i < SizeFrames.size(); i++)
                        {
                            SizeFrames[i].isHovered(mx,my);
                        }
                        break;
                    }
                    default:break;
                }
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                int mx = event.button.x; int my = event.button.y;
                switch(CStage)
                {
                    case Stage::size:
                    {
                        for(int i = 0; i < SizeFrames.size(); i++)
                        {
                            if(SizeFrames[i].GetIsHovered())
                            {
                                
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    void quit()
    {
        plates.clear();
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
    int CurrentTime;
};