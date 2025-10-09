#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include<SDL2/SDL.h>

extern RUI_SceneManager SceneManager;

class RUI_SettingScene:public RUI_Scene
{
    public:
    RUI_SettingScene() = default;
    ~RUI_SettingScene() = default;
            void onEnter()
        {
            //SDL_Log("进入设置场景");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
        void onUpdate()
        {
            //SDL_Log("更新设置场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_Surface* image = IMG_Load("./resources/texture/saving/saving.png");
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);
            SDL_Rect rect = {150,60,500,500};
            SDL_SetRenderDrawColor(Renderer,135,100,235,255);
            SDL_RenderClear(Renderer);

            SDL_RenderCopy(Renderer,texture,nullptr,&rect);

            SDL_RenderPresent(Renderer);

            SDL_FreeSurface(image);
            SDL_DestroyTexture(texture);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                //SDL_Log("切换场景");
                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
            }    
        }
        void onExit()
        {
            SDL_Log("退出设置场景");
        }

};