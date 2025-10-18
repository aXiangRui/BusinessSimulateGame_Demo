#pragma once

#include"RUI_Scene.h"
#include"RUI_Icon.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_CreateEvent.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>

extern RUI_SceneManager SceneManager;

class RUI_CreateScene : public RUI_Scene
{
    public:
    RUI_CreateScene() = default;
    ~RUI_CreateScene() = default;

    MusicPlayer CreateMusic;

    enum class CreateStage
    {
        ChooseSize,
        ChooseBasicMaterial,
        Create,
        ChooseDecorationMaterial
    };

    std::vector<Button> MenuButtons;

    std::vector<int> Size;

    CreateRUIEvent Event;

    void onEnter()
        {
            if(!Mix_PlayingMusic())
            {
                CreateMusic.setMusic(ResourceManager::instance()->FindMusic("createmusic"));
                CreateMusic.play(-1);
            }
            BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回店中",0);
            MenuButtons.push_back(Btn0);
            CurrentStage = CreateStage::ChooseSize;

            for(int i = 0; i < 3; i++)
            {
                Size.push_back( 16* (i+1) );
            }
            Event.load();
            Event.ChooseSize(Size);
            Event.ChooseBase();
            Event.ChooseDecoration();
        }
        void onUpdate()
        {
            //Event.SetStage(StageInt());
            Event.update();
            //SDL_Log("更新设置场景");
            switch(CurrentStage)
            {
                case CreateStage::ChooseSize:{break;}
                case CreateStage::ChooseBasicMaterial:{break;}
                case CreateStage::Create:{break;}
                case CreateStage::ChooseDecorationMaterial:{break; }
                default:break;
            }
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_SetRenderDrawColor(Renderer,10,10,10,255);
            SDL_RenderClear(Renderer);
            SDL_RenderCopy(Renderer, BackgroundTexture, nullptr, &BackgroundRect);

            Event.onRender(Renderer);

            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
            Event.input(event);
        }
        void onExit()
        {
            Size.clear();
            Event.quit();
            CreateMusic.quit();
            SDL_Log("退出创造场景");
        }

        int StageInt()
        {
            switch(CurrentStage)
            {
                case CreateStage::ChooseSize:{return 1;break;}
                case CreateStage::ChooseBasicMaterial:{return 2; break;}
                case CreateStage::Create:{return 3;break;}
                case CreateStage::ChooseDecorationMaterial:{return 4;break;}
                default:{return 0;break;}
            }
        }

    private:
        SDL_Texture* BackgroundTexture;
        SDL_Rect BackgroundRect = {0,0,800,600};
        CreateStage CurrentStage;
};