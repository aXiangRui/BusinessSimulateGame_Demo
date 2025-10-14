#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>
#include<vector>
#include<fstream>
#include"RUI_Scene.h"
#include"RUI_MusicManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Clock.h"
#include"RUI_ResourceManager.h"
#include"RUI_GameEvent.h"

extern RUI_SceneManager SceneManager;
extern MusicPlayer BackgroundMusic;

class RUI_GameScene: public RUI_Scene
{
    public:
        RUI_GameScene() = default;
        ~RUI_GameScene() = default;

        SDL_Texture* Background;

        MusicPlayer gamemusic;
        std::vector<MenuButton> Btns;

        Uint32 CurrentTime;
        Uint32 LastTime;

        Clock TestClock;
        const int HourTime = 1000;

        GameEvent TestEvent;

        void onEnter()
        {
            TestEvent.Load();
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回首页",0);
            Btns.push_back(Btn0);
            BackgroundMusic.quit();
            if(!Mix_PlayingMusic())
            {           
                gamemusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic"));
                gamemusic.play(-1);
            }
            Background = ResourceManager::instance()->FindTexture("hall");

            LastTime = SDL_GetTicks();
            TestClock.SetStartTime(TestEvent.ReturnClockTime());
            SDL_Log("进入游戏场景");
        }
        void onUpdate()
        {
            //SDL_Log("更新游戏场景");
            if(true)
            {  
                TimeChange();
                TestEvent.SetClock(TestClock);
                TestEvent.onUpdate();
            }
            else
            {
                
            }
        }

        void TimeChange()
        {
            int PresentTime = TestClock.ReturnHour();
            if(PresentTime >= 6 && PresentTime < 22)
            {
                CurrentTime = SDL_GetTicks();
                if(CurrentTime - LastTime >= HourTime)
                {          
                    LastTime = CurrentTime;
                    TestClock.UpdateTime();
                }
            }
            else
            {                            
                CurrentTime = SDL_GetTicks();
                if(CurrentTime - LastTime >= HourTime * 2)
                {          
                    LastTime = CurrentTime;
                    TestClock.UpdateTime();
                }
            }
        }

        void onRender(SDL_Renderer* Renderer)
        {
            //SDL_SetRenderDrawColor(Renderer,80,80,235,255);
            SDL_RenderClear(Renderer);

            SDL_Rect BackGroundRect = {0,0,800,600};
            SDL_RenderCopy(Renderer,Background,nullptr,&BackGroundRect);

            for(int i = 0; i < Btns.size(); i++)
            {
                Btns[i].ButtonRender(Renderer);
            }
            TestClock.RenderHour(Renderer);
            TestEvent.onRender(Renderer);
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
            TestEvent.Save();
            gamemusic.quit();
        }
        private:

};