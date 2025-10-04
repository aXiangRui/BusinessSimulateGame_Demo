#pragma once

#include<SDL2/SDL.h>
#include<vector>
#include"RUI_Button.h"
#include"RUI_MenuButton.h"
#include"RUI_Scene.h"
#include"RUI_SceneManager.h"

extern RUI_SceneManager SceneManager;
extern int WindowWidth;


class RUI_MenuScene: public RUI_Scene
{
    public:
        RUI_MenuScene() = default;
        ~RUI_MenuScene() = default;
  
        // Button* Btns[3];
        std::vector<MenuButton>Btns;

        void onEnter()
        {
            SDL_Log(("进入菜单"));
            MenuButton Btn0((WindowWidth-320)/2,350,320,64,"开始游戏",0);
            MenuButton Btn1((WindowWidth-320)/2,420,320,64,"设置",1);
            MenuButton Btn2((WindowWidth-320)/2,490,320,64,"退出游戏",2);
            Btns.push_back(Btn0);
            Btns.push_back(Btn1);
            Btns.push_back(Btn2);
            // Btns[0] = new MenuButton((WindowWidth-320)/2,350,320,64,"开始游戏",0);
            // Btns[1] = new MenuButton((WindowWidth-320)/2,420,320,64,"设置",1);
            // Btns[2] = new MenuButton((WindowWidth-320)/2,490,320,64,"退出游戏",2);
        }
        void onUpdate()
        {
            SDL_Log("更新菜单场景");
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_Log("渲染菜单中");

            SDL_SetRenderDrawColor(Renderer,105,100,235,255);
            SDL_RenderClear(Renderer);
            for(int i = 0; i < Btns.size(); i++)
            {
                if(Btns[i].getClicked() == true)
                {
                    //SDL_Log("%d正在渲染",i);
                    Btns[i].ClickedButtonRender(Renderer);
                    // SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                }
                else if(Btns[i].getHovered() == true)
                {
                    //SDL_Log("%d正在渲染",i);
                    Btns[i].HoveredButtonRender(Renderer);
                    // SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                }
                else
                {
                    //SDL_Log("%d正在渲染",i);
                    Btns[i].ButtonRender(Renderer);
                }
                
            }
                
            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer,bool& running)
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
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                                    break;
                                }
                                case 1:
                                {
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Setting);
                                    break;
                                }
                                case 2:
                                {
                                    running = false;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            Btns[i].setClicked(false);
                        }
                        
                    }
                    // SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    
                    int mx = event.motion.x;
                    int my = event.motion.y;
                    for(int i = 0; i < Btns.size(); i++)
                    {
                        if(Btns[i].RUI_isHovered(mx,my))
                        {
                            Btns[i].setHovered(true);
                        }
                        else
                        {
                            Btns[i].setHovered(false);
                            Btns[i].setClicked(false);
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
            SDL_Log("退出菜单场景");
            Btns.clear();
        }


        void setBtns()
        {

        }

        private:

};