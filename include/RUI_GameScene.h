#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>
#include<queue>
#include<vector>
#include<fstream>
#include"RUI_Icon.h"
#include"RUI_DessertManager.h"
#include"RUI_CustomerManager.h"
#include"RUI_Chair.h"
#include"RUI_Scene.h"
#include"RUI_MusicManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Clock.h"
#include"RUI_ResourceManager.h"
#include"RUI_GameEvent.h"
#include"RUI_Cabinet.h"
#include"RUI_ChatFrame.h"

extern RUI_SceneManager SceneManager;
extern MusicPlayer BackgroundMusic;

class RUI_GameScene: public RUI_Scene
{
    public:
        RUI_GameScene() = default;
        ~RUI_GameScene() = default;

        SDL_Texture* Background;
        SDL_Texture* BackgroundWall;
        TTF_Font* TextFont;
        CustomerManager customerManager;
        DessertManager dessertManager;

        MusicPlayer gamemusic;
        std::vector<MenuButton> Btns;
        std::vector<Chair> Chairs;
        std::vector<Desk> Desks;
        std::vector<Cabinet> Cabinets;
        std::queue<ChatFrame> ChatFrames;
        std::vector<RUI_Icon> Icons;
        Register reg;

        Uint32 CurrentTime;
        Uint32 LastTime;

        Clock TestClock;
        const int HourTime = 10000;

        int TotalMoney = 0;
        int isChatFrameShowing = 0;

        int TotalCustomers;

        GameEvent TestEvent;

        void onEnter()
        {
            TestEvent.Load(TotalMoney,TotalCustomers);
            // MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回首页",0);
            // Btns.push_back(Btn0);
            BackgroundMusic.quit();
            reg.InitRegister();
            if(!Mix_PlayingMusic())
            {           
                gamemusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic"));
                gamemusic.play(-1);
            }
            customerManager.InitCustomerManager();
            dessertManager.InitDessertManager();
            Background = ResourceManager::instance()->FindTexture("hall");
            BackgroundWall = ResourceManager::instance()->FindTexture("hallwall");
            TextFont = nullptr;

            for(int i = 0; i < 16; i++)
            {
                Chair chair;
                chair.InitChair(i);
                Chairs.push_back(chair);
            }

            for(int i = 0; i < 24; i++)
            {
                Cabinet cabinet;
                cabinet.InitCabinet(i);
                Cabinets.push_back(cabinet);
            }

            for(int i = 0; i < 8; i++)
            {
                Desk desk;
                desk.initDesk(i);
                Desks.push_back(desk);
            }

            while(ChatFrames.size()!=0)
            {
                ChatFrames.pop();
            }

            RUI_Icon testicon;
            testicon.InitIcon(10,10,50,50,0,"sun");
            Icons.push_back(testicon);

            RUI_Icon CookingIcon;
            CookingIcon.InitIcon(10,430,50,50,1,"cooking");
            Icons.push_back(CookingIcon);

            RUI_Icon exiticon;
            exiticon.InitIcon(10,500,50,50,2,"exiticon");
            Icons.push_back(exiticon);

            LastTime = SDL_GetTicks();
            TestClock.SetStartTime(TestEvent.ReturnClockTime());
            SDL_Log("进入游戏场景");
        }
        void onUpdate()
        {
            //SDL_Log("更新游戏场景");
            if(ChatFrames.size() == 0)
            {  
                TimeChange();
                TestEvent.SetClock(TestClock);
                TestEvent.onUpdate(Chairs,Cabinets,customerManager,dessertManager,TotalMoney,TotalCustomers);
            }
            else
            {
                isChatFrameShowing = 1;

            }
        }

        void TimeChange()
        {
            int PresentTime = TestClock.ReturnHour();
            if(PresentTime >= 7 && PresentTime < 22)
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
                if(TestEvent.GetCustomerNumber() == 0)
                {      
                    if(CurrentTime - LastTime >= HourTime / 10)
                    {          
                        LastTime = CurrentTime;
                        TestClock.UpdateTime();
                    }
                }
                else
                {
                    if(CurrentTime - LastTime >= HourTime * 10)
                    {          
                        LastTime = CurrentTime;
                        TestClock.UpdateTime();
                    }
                }
                
            }
        }

        void onRender(SDL_Renderer* Renderer)
        {

            if(!TextFont)
                TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            SDL_Color color = { 10, 10, 10, 255};
            std::string Title = "总金额" + std::to_string(TotalMoney);
            SDL_Surface* image = TTF_RenderUTF8_Blended(TextFont, Title.c_str(),color);
            SDL_Rect TextRect = {10,60,image->w,image->h};
            SDL_Texture* MoneyTexture = SDL_CreateTextureFromSurface(Renderer,image);
            SDL_FreeSurface(image);

            //SDL_SetRenderDrawColor(Renderer,80,80,235,255);
            SDL_RenderClear(Renderer);

            SDL_Rect BackGroundRect = {0,0,800,600};
            SDL_RenderCopy(Renderer,Background,nullptr,&BackGroundRect);
            SDL_RenderCopy(Renderer,MoneyTexture,nullptr,&TextRect);

            for(int i = 0; i < Btns.size(); i++)
            {
                Btns[i].ButtonRender(Renderer);
            }

            for(int i = 0; i < Chairs.size(); i++)
            {
                Chairs[i].onRender(Renderer);
            }

            for(int i = 0; i < Desks.size(); i++)
            {
                Desks[i].onRender(Renderer);
            }

            reg.onRender(Renderer);

            TestClock.RenderHour(Renderer);
            TestEvent.onRender(Renderer);
 
            for(int i = 0; i < Cabinets.size(); i++)
            {
                Cabinets[i].onRender(Renderer);
            } 

            for(int i = 0; i < Icons.size(); i++)
            {
                Icons[i].onRender(Renderer);
            }

            if(isChatFrameShowing)
            {

            }
            // SDL_Rect BackGroundWallRect = {0,6,800,600};
            // SDL_RenderCopy(Renderer,BackgroundWall,nullptr,&BackGroundWallRect);

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
                    for(int i = 0; i < Icons.size(); i++)
                    {
                        if(Icons[i].isClicked(mx,my))
                        {
                            switch(i)
                            {
                                case 1:
                                {
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Create);
                                    break;
                                }
                                case 2:
                                {
                                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
                                    break;
                                }
                                default:
                                break;
                            }
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
                    int k = 0;
                    for(int i = 0; i < Icons.size(); i++)
                    {
                        if(Icons[i].isHovered(mx,my))
                        {
                            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            k = 1;
                        }
                        if(!k)
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
            Btns.clear();
            Chairs.clear();
            Desks.clear();
            Cabinets.clear();
            Icons.clear();
            TestEvent.Save(TotalMoney,TotalCustomers);
            gamemusic.quit();
        }
        private:

};