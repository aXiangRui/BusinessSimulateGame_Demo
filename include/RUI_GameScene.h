#pragma once

#include<SDL2/SDL.h>
#include"RUI_Scene.h"

class RUI_GameScene: public RUI_Scene
{
    public:
        RUI_GameScene() = default;
        ~RUI_GameScene() = default;
        void onEnter()
        {
            SDL_Log("进入游戏场景");
        }
        void onUpdate()
        {
            SDL_Log("更新游戏场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_SetRenderDrawColor(Renderer,135,100,235,255);
            SDL_RenderClear(Renderer);
            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event)
        {
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Log("切换场景");
                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
            }    
        }
        void onExit()
        {
            SDL_Log("退出游戏场景");
        }
        private:

};