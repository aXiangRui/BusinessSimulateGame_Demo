#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_BaseMaterial.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_ChooseFrame.h"
#include"RUI_Product.h"
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
    std::vector<Dessert> BaseDessert;
    std::vector<ChooseFrame> DecorationFrames;
    std::vector<Plate> plates;
    ProducingProduct PProduct;

    void load()
    {
        CStage = Stage::size;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");     
        dessertManager.InitDessertManager();

        for(int i = 0; i < 3; i++)
        {
            Plate a;
            a.InitPlate(140,-60,i);
            plates.push_back(a);
        }
        PProduct.Init();
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

    void ChooseBase()
    {
        for(int i = 0; i < dessertManager.GetDessertsSize(); i++)
        {
            ChooseFrame AddFrames;
            std::string name = dessertManager.GetDessertName(i);
            AddFrames.InitChooseFrame(500, 10 + i * 50, i, name.c_str());
            BaseFrames.push_back(AddFrames);
            BaseDessert.push_back(dessertManager.Desserts[i]);
            BaseDessert[i].SetPosition(140,-100);
        }
    }

    void update()
    {
        CurrentTime = SDL_GetTicks();
        if(PProduct.GetPlateSize() != -1)
        {
            plates[PProduct.GetPlateSize()].MovitionUpdate(CurrentTime,5);
        }
        if(PProduct.GetBaseID() != -1)
        {
            BaseDessert[PProduct.GetBaseID()].moveUpdate(CurrentTime);
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {    
        if(PProduct.GetPlateSize() != -1)
            plates[PProduct.GetPlateSize()].onRender(Renderer);
        if(PProduct.GetBaseID() != -1)
            BaseDessert[PProduct.GetBaseID()].onRender(Renderer);
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
        case Stage::base:
        {
            for(int i = 0; i < BaseFrames.size(); i++)
            {
                if(BaseFrames[i].GetIsHovered())
                {
                    BaseFrames[i].onHoverRender(Renderer);
                    BaseDessert[i].onRender(Renderer);
                }
                else
                {
                    BaseFrames[i].onRender(Renderer);
                }
                
            }
            break;
        }
        case Stage::create:
        {
            PProduct.RenderCreateNumbers(Renderer);
        }
        default:break;
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
                        int j = 0;
                        for(int i = 0; i < SizeFrames.size(); i++)
                        {
                            if(SizeFrames[i].isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }                        
                        }
                        if(!j)
                            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                        break;
                    }
                    case Stage::base:
                    {
                        int j = 0;
                        for(int i = 0; i < BaseFrames.size(); i++)
                        {
                            if(BaseFrames[i].isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }
                        }
                        if(!j)
                            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                        break;
                    }
                    default:break;
                }
                break;
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
                                //SDL_Log("当前按下的坐标:%d,%d,%d",mx,my,i);
                                plates[i].SetMoveCheck(1);
                                PProduct.SetPlateSize(i);
                                CStage = Stage::base;
                            }
                        }
                        break;        
                    }
                    case Stage::base:
                    {
                        for(int i = 0; i < BaseFrames.size(); i++)
                        {
                            if(BaseFrames[i].GetIsHovered())
                            {
                                BaseDessert[i].SetIsClicked(1);
                                PProduct.SetBaseID(BaseDessert[i].GetDessertID());
                                CStage = Stage::create;
                            }
                        }
                    }
                    default:break;
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
        BaseFrames.clear();
        BaseDessert.clear();
        dessertManager.quit();
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
    DessertManager dessertManager;
};