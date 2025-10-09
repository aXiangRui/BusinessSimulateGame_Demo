#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

extern RUI_SceneManager SceneManager;
extern SDL_Renderer* Renderer;

class RUI_SavingScene : public RUI_Scene
{
    public:

    RUI_SavingScene() = default;
    ~RUI_SavingScene() = default;

    std::vector<MenuButton> Btns;
    void onEnter()
        {
            SDL_Log("进入存档场景");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回首页",0);
            MenuButton Btn1((WindowWidth-320)/2,120,320,64,"开始游戏",1);
            Btns.push_back(Btn0);
            Btns.push_back(Btn1);

        }
        void onUpdate()
        {
            //SDL_Log("更新存档场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_Surface* image = IMG_Load("./resources/texture/saving/saving.png");
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer,image);
            SDL_Rect rect = {150,60,500,500};
            SDL_FreeSurface(image);
            SDL_SetRenderDrawColor(Renderer,135,100,235,255);
            SDL_RenderClear(Renderer);
          
            SDL_RenderCopy(Renderer,texture,nullptr,&rect);
            SDL_DestroyTexture(texture);
            for(int i = 0; i < Btns.size(); i++)
            {
                if(Btns[i].getClicked() == true)
                {
                    Btns[i].ClickedButtonRender(Renderer);
                }
                else if(Btns[i].getHovered() == true)
                {
                    Btns[i].HoveredButtonRender(Renderer);
                }
                else
                {
                    Btns[i].ButtonRender(Renderer);                  
                }
            }
            
            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
            switch(event.type)
            {               
                case SDL_MOUSEBUTTONDOWN:
                {            
                    int mx = event.motion.x;
                    int my = event.motion.y;
                    for(int i = 0; i < Btns.size(); i++)
                    {
                        if(Btns[i].RUI_isClicked(mx,my))
                        {
                            Btns[i].setClicked(true);
                            switch(i)
                            {
                                case 0:
                                {
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
                                    break;
                                }
                                case 1:
                                {
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                                    break;
                                }
                                default:
                                break;
                            }
                        }
                        else
                        {
                            Btns[i].setClicked(false);
                        }
                        
                    }
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    
                    int mx = event.motion.x;
                    int my = event.motion.y;
                    int j = 0;
                    for(int i = 0; i < Btns.size(); i++)
                    {
                        if(Btns[i].RUI_isHovered(mx,my))
                        {
                            Btns[i].setHovered(true);
                            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            j = 1;
                        }
                        else
                        {
                            Btns[i].setHovered(false);
                            Btns[i].setClicked(false);
                        }
                        if(!j)
                        {
                            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                        }
                    }
                    break;
                }
                default:
                    break;
            }   
        }
        void onExit()
        {
            Btns.clear();
            SDL_Log("退出存档场景");
        }
};