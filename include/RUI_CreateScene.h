#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>

extern RUI_SceneManager SceneManager;

class RUI_CreateScene:public RUI_Scene
{
    public:
    RUI_CreateScene() = default;
    ~RUI_CreateScene() = default;

    MusicPlayer CreateMusic;

    enum class CreateStage
    {
        ChooseSize,
        ChooseBasicMaterial,
        ChooseDecorationMaterial
    };

    std::vector<Button> MenuButtons;

    void onEnter()
        {
            //SDL_Log("进入设置场景");
            if(!Mix_PlayingMusic())
            {
                CreateMusic.setMusic(ResourceManager::instance()->FindMusic("createmusic"));
                CreateMusic.play(-1);
            }
            BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回店中",0);
            MenuButtons.push_back(Btn0);
        }
        void onUpdate()
        {
            //SDL_Log("更新设置场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_SetRenderDrawColor(Renderer,10,10,10,255);
            SDL_RenderClear(Renderer);
            SDL_RenderCopy(Renderer, BackgroundTexture, nullptr, &BackgroundRect);
            
            for(int i = 0; i < MenuButtons.size(); i++)
            {
                MenuButtons[i].ButtonRender(Renderer);
            }
            
            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
            }    
        }
        void onExit()
        {
            CreateMusic.quit();
            SDL_Log("退出创造场景");
        }

    private:
        SDL_Texture* BackgroundTexture;
        SDL_Rect BackgroundRect = {0,0,800,600};

};