#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>
#include<vector>
#include"RUI_Scene.h"
#include"RUI_MusicManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Clock.h"

extern RUI_SceneManager SceneManager;
extern MusicPlayer BackgroundMusic;

class RUI_GameScene: public RUI_Scene
{
    public:
        RUI_GameScene() = default;
        ~RUI_GameScene() = default;

        Mix_Music* music;
        std::vector<MenuButton> Btns;

        Uint32 CurrentTime;
        Uint32 LastTime;

        Clock TestClock;

        void onEnter()
        {
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回首页",0);
            Btns.push_back(Btn0);
            BackgroundMusic.quit();
            LastTime = SDL_GetTicks();
            TestClock.SetStartTime(5);
            SDL_Log("进入游戏场景");
        }
        void onUpdate()
        {
            //SDL_Log("更新游戏场景");
            CurrentTime = SDL_GetTicks();
            // SDL_Log("%d %d",CurrentTime,LastTime);
            if(CurrentTime - LastTime >= 1000)
            {
                //SDL_Log("时间过去1秒");               
                LastTime = CurrentTime;
                TestClock.UpdateTime();
                SDL_Log("%d",TestClock.ReturnHour());
            }
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_SetRenderDrawColor(Renderer,135,100,235,255);
            SDL_RenderClear(Renderer);
            for(int i = 0; i < Btns.size(); i++)
            {
                Btns[i].ButtonRender(Renderer);
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
            SDL_Log("退出游戏场景");
        }
        private:

};