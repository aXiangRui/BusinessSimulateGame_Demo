#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

extern RUI_SceneManager SceneManager;

class RUI_CreateScene:public RUI_Scene
{
    public:
    RUI_CreateScene() = default;
    ~RUI_CreateScene() = default;
    void onEnter()
        {
            //SDL_Log("进入设置场景");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回首页",0);
        }
        void onUpdate()
        {
            //SDL_Log("更新设置场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_RenderClear(Renderer);
            SDL_SetRenderDrawColor(Renderer,10,10,10,255);
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
            SDL_Log("退出创造场景");
        }

};