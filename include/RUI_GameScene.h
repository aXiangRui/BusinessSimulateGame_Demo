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
        CabinetFrame cabinetFrame;
        std::queue<ChatFrame> ChatFrames;
        std::vector<RUI_Icon> Icons;
        Register reg;

        Uint32 CurrentTime;
        Uint32 LastTime;

        Clock TestClock;
        const int HourTime = 10000;

        int TotalMoney = 0;
        int isChatFrameShowing = 0;
        bool WhetherReadingProduct;
        bool isSettingNewProduct;
        bool CheckSetting;

        int TotalCustomers;
        int ReadingPage;
        int CurrentCabnet;

        GameEvent TestEvent;

        void onEnter()
        {
            TestEvent.Load(TotalMoney,TotalCustomers,Cabinets);
            TestEvent.onEnter();
            TestEvent.SetIsReadingPage(0);
            MenuButton Btn0((WindowWidth-320)/2,450,320,64,"设置新甜点",0);
            Btns.push_back(Btn0);
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
            cabinetFrame.InitFrame();

            for(int i = 0; i < 16; i++)
            {
                Chair chair;
                chair.InitChair(i);
                Chairs.push_back(chair);
            }

            // for(int i = 0; i < 24; i++)
            // {
            //     Cabinet cabinet;
            //     cabinet.InitCabinet(i);
            //     Cabinets.push_back(cabinet);
            // }

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

            RUI_Icon readicon;
            readicon.InitIcon(10,360,50,50,3,"readicon");
            Icons.push_back(readicon);

            RUI_Icon nexticon;
            nexticon.InitIcon(675,250,50,50,4,"nexticon");
            Icons.push_back(nexticon);

            RUI_Icon lasticon;
            lasticon.InitIcon(75,250,50,50,5,"nexticon");
            Icons.push_back(lasticon);

            LastTime = SDL_GetTicks();
            TestClock.SetStartTime(TestEvent.ReturnClockTime());

            WhetherReadingProduct = 0;
            isSettingNewProduct = 0;
            CheckSetting = 0;
            ReadingPage = -1;
            CurrentCabnet = -1;
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
                if(i <= 3)
                    Icons[i].onRender(Renderer);
                    else
                    {
                        if(TestEvent.GetIsReadingPage())
                        {
                            if(i == 4){Icons[i].onRender(Renderer);}
                            if(i == 5){Icons[i].onRender(Renderer,1);} 
                        }
                    }        
            }
            if(cabinetFrame.GetCabinetID() != -1)
            {
                //SDL_Log("当前面包柜id%d",cabinetFrame.GetCabinetID());
                //cabinetFrame.onRender(Renderer, Cabinets[cabinetFrame.GetCabinetID()],produtManager);
                TestEvent.onFrameRender(Renderer,cabinetFrame, Cabinets);
            }

            if(WhetherReadingProduct)
            {
                TestEvent.onProductRender(Renderer, ReadingPage);
            }
      
            for(int i = 0; i < Btns.size(); i++)
            {
                if(isSettingNewProduct)
                    Btns[i].ButtonRender(Renderer);
            }

            if(CheckSetting == 1)
            {
                TestEvent.SettingProductRender(Renderer,ReadingPage);
                Icons[4].onRender(Renderer);
                Icons[5].onRender(Renderer,1);
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
                    if(CheckSetting)
                    {
                        if(CheckRect(200,400,0,200,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                        if(CheckRect(200,400,200,400,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6+1);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                        if(CheckRect(200,400,400,600,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6+2);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                        if(CheckRect(400,600,0,200,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6+3);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                        if(CheckRect(400,600,200,400,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6+4);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                        if(CheckRect(400,600,400,600,mx,my))
                        {
                            Cabinets[CurrentCabnet].SetDessertID(ReadingPage*6+5);
                            CheckSetting = 0;
                            isSettingNewProduct = 1;
                        }
                    }
                    for(int i = 0; i < Btns.size(); i++)
                    {
                        if(isSettingNewProduct)
                        {
                            if(Btns[i].RUI_isClicked(mx,my))
                            {
                                Btns[i].setClicked(true);
                                switch(i)
                                {
                                    case 0:
                                    {
                                        CheckSetting = 1;
                                        ReadingPage = 0;
                                        isSettingNewProduct = 0;
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
                                case 3:
                                {
                                    if(WhetherReadingProduct == 0)
                                    {
                                        WhetherReadingProduct = 1;
                                        TestEvent.SetIsReadingPage(1);
                                        ReadingPage = 0;
                                    }
                                    else
                                    {
                                        WhetherReadingProduct = 0;
                                        TestEvent.SetIsReadingPage(0);
                                        ReadingPage = -1;
                                    }                                       
                                    break;
                                }
                                case 4:
                                {
                                    if(TestEvent.GetIsReadingPage())
                                    {
                                        ReadingPage = ReadingPage + 1;
                                        if(ReadingPage > TestEvent.GetProductNumber()/6)
                                        {
                                            ReadingPage = TestEvent.GetProductNumber() / 6;
                                        }
                                    }
                                    if(CheckSetting)
                                    {                        
                                        ReadingPage = ReadingPage + 1;
                                        if(ReadingPage > TestEvent.GetProductNumber()/6)
                                        {
                                            ReadingPage = TestEvent.GetProductNumber() / 6;
                                        }
                                    }
                                    break;
                                }
                                case 5:
                                {
                                    if(TestEvent.GetIsReadingPage())
                                    {
                                        ReadingPage = ReadingPage - 1;
                                        if(ReadingPage < 0)
                                            ReadingPage = 0;
                                    }
                                    if(CheckSetting)
                                    { 
                                        ReadingPage = ReadingPage - 1;
                                        if(ReadingPage < 0)
                                            ReadingPage = 0;
                                    }
                                }
                                default:
                                break;
                            }
                        }
                    }
                    for(int i = 0; i < Cabinets.size(); i++)
                    {
                        if(isSettingNewProduct == 0)
                        {
                            if(Cabinets[i].isClicked(mx, my))
                            {
                                CurrentCabnet = i;
                                cabinetFrame.SetCabinetID(i);
                                isSettingNewProduct = 1;
                            }
                        }
                    }
                    if(cabinetFrame.GetCabinetID() != -1)
                    {
                        if(mx >= 580 && mx <= 612)
                        {
                            if(my >= 100 && my <= 132)
                            {
                                cabinetFrame.SetCabinetID(-1);
                                isSettingNewProduct = 0;
                                cabinetFrame.quit();
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
                    if(CheckSetting)
                    {
                        if(mx >= 200 && mx <= 600)
                        {
                            if(my >= 0 & my <= 600)
                            {
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                j = 1;
                            }
                        }
                    }
                    for(int i = 0; i < Btns.size(); i++)
                    {
                        if(isSettingNewProduct)
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
                        }
                    }
                    for(int i = 0; i < Icons.size(); i++)
                    {
                        if(Icons[i].isHovered(mx,my))
                        {
                            if(i <= 3)
                            {
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                j = 1;
                            }    
                            else
                            {
                                if(TestEvent.GetIsReadingPage())
                                {
                                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                    j = 1;
                                }
                            }
                        }
                    }
                    for(int i = 0; i < Cabinets.size(); i++)
                    {
                        if(CheckSetting == 0)
                        {
                            if(Cabinets[i].isClicked(mx,my))
                            {
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                j = 1;
                            }
                        }
                    } 

                    if(cabinetFrame.GetCabinetID() != -1)
                    {
                        if(mx >= 580 && mx <= 612)
                        {
                            if(my >= 100 && my <= 132)
                            {
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                j = 1;
                            }
                        }
                    } 
                    
                    if(!j)
                    {
                        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                    }
                    break;
                }
                default:
                    break;
            }
            TestEvent.input(event);
        }
        void onExit()
        {
            SDL_Log("退出游戏场景");          
            TestEvent.Save(TotalMoney,TotalCustomers, Cabinets);
            Btns.clear();
            Chairs.clear();
            Desks.clear();
            Cabinets.clear();
            Icons.clear();
            TestEvent.quit();
            dessertManager.quit();
            gamemusic.quit();
        }

        bool CheckRect(int sx, int ex, int sy, int ey, int mx, int my)
        {
            if(mx >= sx && mx <= ex && my >= sy && my <= ey)
            {
                return true;
            }
            return false;
        }
        private:

};