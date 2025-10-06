#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

extern RUI_SceneManager SceneManager;
extern SDL_Renderer* Renderer;

class RUI_SavingScene : public RUI_Scene
{
    public:

    RUI_SavingScene() = default;
    ~RUI_SavingScene() = default;

    void onEnter()
        {
            SDL_Log("进入存档场景");

        }
        void onUpdate()
        {
            SDL_Log("更新存档场景");
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
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Log("切换场景");
                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
            }    
        }
        void onExit()
        {
            SDL_Log("退出存档场景");
        }
};