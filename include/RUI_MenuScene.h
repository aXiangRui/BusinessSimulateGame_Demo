#pragma once

#include<SDL2/SDL.h>
#include<vector>
#include"RUI_Button.h"
#include"RUI_MenuButton.h"
#include"RUI_Scene.h"
#include"RUI_SceneManager.h"

extern RUI_SceneManager SceneManager;

class RUI_MenuScene: public RUI_Scene
{
    public:
        RUI_MenuScene() = default;
        ~RUI_MenuScene() = default;
  
        Button* Btns[3];

        void onEnter()
        {
            Btns[0] = new MenuButton(50,50,320,64,"test",0);
            Btns[1] = new MenuButton(50,150,320,64,"test2",1);
            Btns[2] = new MenuButton(50,250,320,64,"test3",2);
        }
        void onUpdate()
        {
            SDL_Log("更新菜单场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_Log("渲染菜单中");
            // SDL_Surface* image = IMG_Load("./resources/texture/button/buttonDemo.png");
            // SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);
            // SDL_Rect des1 = {80,50,640,128};
            // SDL_Rect des2 = {240,250,320,64};

            SDL_SetRenderDrawColor(Renderer,105,100,235,255);
            SDL_RenderClear(Renderer);
            // SDL_RenderCopy(Renderer,texture,nullptr,&des1);
            // SDL_RenderCopy(Renderer,texture,nullptr,&des2);
            for(int i = 0; i < 3; i++)
                Btns[i]->ButtonRender(Renderer);
            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event)
        {
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Log("切换场景");
                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
            }    
        }
        void onExit()
        {
            SDL_Log("退出菜单场景");
        }


        void setBtns()
        {

        }

        private:

};