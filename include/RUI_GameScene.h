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
#include"RUI_MaterialManager.h"
#include"RUI_GameEvent.h"
#include"RUI_Cabinet.h"
#include"RUI_ChatFrame.h"
#include"RUI_TextManager.h"
#include"RUI_CheckUpdate.h"
#include"RUI_SceneManager.h"

extern RUI_SceneManager SceneManager;
extern MusicPlayer BackgroundMusic;
extern int WindowWidth;

class RUI_GameScene: public RUI_Scene
{
    public:
        RUI_GameScene() = default;
        ~RUI_GameScene() = default;

        void onEnter()
        {               
            customerManager.InitCustomerManager();  
            materialManager.InitMaterialManager();
            TestEvent.Load(TotalMoney,TotalCustomers, TotalDessert, Cabinets, customerManager);
            TestEvent.onEnter();
            TestEvent.SetIsReadingPage(0);
            MenuButton Btn0((WindowWidth-320)/2,450,320,64,"设置新甜点",0);
            Btns.push_back(Btn0);
            BackgroundMusic.quit();
            reg.InitRegister();
            unlockFrame.init();
            Icons.Init();
            if(!Mix_PlayingMusic())
            {
                if(rand() % 2 == 1) 
                {
                    gamemusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic"));
                    gamemusic.play(-1);
                }        
                else
                {
                    gamemusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic02"));
                    gamemusic.play(-1);
                } 
            }            
            dessertManager.InitDessertManager(); 
            textManager.Init();         
            Background = ResourceManager::instance()->FindTexture("hall");
            BackgroundWall = ResourceManager::instance()->FindTexture("hallwall");
            NightTexture = ResourceManager::instance()->FindTexture("night");
            TextFont = nullptr;
            cabinetFrame.InitFrame();
            summaryFrame.Init();
            CheckEvent.init();

            for(int i = 0; i < 16; i++)
            {
                Chair chair;
                chair.InitChair(i);
                Chairs.push_back(chair);
            }

            for(int i = 0; i < 8; i++)
            {
                Desk desk;
                desk.initDesk(i);
                Desks.push_back(desk);
            }

            LastTime = SDL_GetTicks();
            TestClock.SetStartTime(TestEvent.ReturnClockTime());
            materialFrame.Init();

            WhetherReadingProduct = 0;
            isSettingNewProduct = 0;
            CheckSetting = 0;
            ReadingPage = -1;
            CurrentCabnet = -1;
            ChatDelayTime = 0;
            isMaterialFrameShowing = 0;
            currentalpha = 0;
            NightChanged = 0;

            BackGroundRect = {0,0,800,600};
            
            SDL_Log("进入游戏场景");
        }

        void onUpdate()
        {
            CurrentTime = SDL_GetTicks();
            // SDL_Log("更新游戏场景");
            if(isChatFrameShowing == 0)
            {  
                TimeChange();
                TestEvent.SetClock(TestClock);
                TestEvent.onUpdate(Chairs,
                    Cabinets,customerManager,
                    dessertManager,TotalMoney,
                    TotalCustomers,TotalDessert, TestClock.ReturnHour());
            }
            else
            {
                if(ChatDelayTime == 0)
                {
                    ChatDelayTime = CurrentTime;
                }
                if(CurrentTime - ChatDelayTime >= 5000)
                {
                    isChatFrameShowing = 0;
                    ChatDelayTime = 0;
                }
            }
            std::vector<Customer>customers = TestEvent.GetCustomers();
            CheckEvent.update(customerManager, dessertManager, materialManager, chatFrame, textManager, unlockFrame, isChatFrameShowing,customers);
            TestEvent.SetCustomers(customers);
            unlockFrame.SetTime(CurrentTime);
        }

        void TimeChange()
        {
            int PresentTime = TestClock.ReturnHour();
            if( PresentTime >= 18 && NightChanged == 0)
            {
                currentalpha = ( PresentTime - 17 ) * 30;
                if( currentalpha  > 150)
                {
                    currentalpha = 150;
                } 
                NightChanged = 1;
            }
            else if( PresentTime <= 7 && NightChanged == 0)
            {
                currentalpha =  (8 - PresentTime) * 30;
                if( currentalpha  > 150)
                {
                    currentalpha = 150;
                } 
                NightChanged = 1;
            }
            else if( NightChanged == 0)
            {
                currentalpha = 0;
            }
            CurrentTime = SDL_GetTicks();

            if(PresentTime >= 7 && PresentTime < 22)
            {
                CurrentTime = SDL_GetTicks();
                if(CurrentTime - LastTime >= HourTime)
                {          
                    LastTime = CurrentTime;
                    TestClock.UpdateTime();
                    NightChanged = 0;
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
                        if(TestClock.ReturnHour() == 0)
                        {     
                            summaryFrame.update(TotalCustomers,TotalDessert);
                            isSummaryShowing = 1;
                            TotalMoney = TotalMoney - 10 * TotalDessert;
                            SDL_Log("今日卖出甜品%d份",TotalDessert);
                            SDL_Log("今日顾客共有%d人",TotalCustomers);   
                            for(int i = 0; i < Cabinets.size(); i++)
                            {
                                if(Cabinets[i].GetDessertNumber() < 20)
                                {
                                    TotalMoney  -= 10 * (20 - Cabinets[i].GetDessertNumber());
                                    Cabinets[i].SetDessertNumber(20);
                                }
                            }  
                            for(int i = 0; i < 16; i++)
                            {
                                Chairs[i].SetUsing(0);
                            }                      
                        }
                        if(TestClock.ReturnHour() == 6)
                        {
                            TotalDessert = 0;
                            TotalCustomers = 0;
                            isSummaryShowing = 0;
                        } 
                        NightChanged = 0;
                    }
                }
                else
                {
                    if(CurrentTime - LastTime >= HourTime * 10)
                    {          
                        LastTime = CurrentTime;
                        TestClock.UpdateTime();
                        NightChanged = 0;
                    }
                }
                
            }
        }

        void onRender(SDL_Renderer* Renderer)
        {

            if(!TextFont)
                TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            
            if(TotalMoney < 0)
            color = {200, 40, 40, 255};
            std::string Title = "总金额" + std::to_string(TotalMoney);
            SDL_Surface* image = TTF_RenderUTF8_Blended(TextFont, Title.c_str(),color);
            SDL_Rect TextRect = {10,60,image->w,image->h};
            SDL_Texture* MoneyTexture = SDL_CreateTextureFromSurface(Renderer,image);
            SDL_FreeSurface(image);

            //SDL_SetRenderDrawColor(Renderer,80,80,235,255);
            SDL_RenderClear(Renderer);

            SDL_RenderCopy(Renderer,Background,nullptr,&BackGroundRect);
            SDL_RenderCopy(Renderer,MoneyTexture,nullptr,&TextRect);

            SDL_DestroyTexture(MoneyTexture);

            for(int i = 0; i < Chairs.size(); i++)
            {
                Chairs[i].onRender(Renderer);
            }

            for(int i = 0; i < Desks.size(); i++)
            {
                Desks[i].onRender(Renderer);
            }

            reg.onRender(Renderer);

            TestEvent.onRender(Renderer);
            
            SDL_SetTextureAlphaMod(NightTexture, currentalpha);
            SDL_RenderCopy(Renderer,NightTexture, nullptr, &BackGroundRect);

            TestClock.RenderHour(Renderer);
 
            for(int i = 0; i < Cabinets.size(); i++)
            {
                Cabinets[i].onRender(Renderer);
            } 

            bool a = TestEvent.GetIsReadingPage();
            Icons.onRender(Renderer, a);

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
                if(isSettingNewProduct && WhetherReadingProduct == 0)
                    Btns[i].ButtonRender(Renderer);
            }

            if( isMaterialFrameShowing == 1)
            {
                materialFrame.onRender(Renderer);
            }

            if(CheckSetting == 1)
            {
                TestEvent.SettingProductRender(Renderer,ReadingPage);
                Icons.Icons[4].onRender(Renderer);
                Icons.Icons[5].onRender(Renderer,1);
            }

            if(isSummaryShowing)
            {
                summaryFrame.onRender(Renderer);
            }

            unlockFrame.onRender(Renderer);

            if(isChatFrameShowing)
            {
                chatFrame.RenderFrame(Renderer);
                chatFrame.RenderTitle(Renderer);
                chatFrame.RenderContent(Renderer);
            }
            if(TestEvent.GetWhetherRenderCustomerFrame())
            {
                TestEvent.onCustomerRender(Renderer);
            }
            // SDL_Rect BackGroundWallRect = {0,6,800,600};
            // SDL_RenderCopy(Renderer,BackgroundWall,nullptr,&BackGroundWallRect);

            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running);

        void onExit()
        {
            SDL_Log("退出游戏场景");        
            customerManager.Save();  
            // customerManager.Reset();
            materialManager.Save();
            materialManager.quit();
            TestEvent.Save(TotalMoney,TotalCustomers, TotalDessert, Cabinets);
            Btns.clear();
            Chairs.clear();
            Desks.clear();
            Cabinets.clear();
            Icons.Quit();
            TestEvent.quit();
            dessertManager.Save();
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
        SDL_Texture* Background;
        SDL_Texture* BackgroundWall;
        SDL_Texture* NightTexture;
        TTF_Font* TextFont;
        CustomerManager customerManager;
        DessertManager dessertManager;
        TextManager textManager;
        MaterialManager materialManager;

        MusicPlayer gamemusic;
        std::vector<MenuButton> Btns;
        std::vector<Chair> Chairs;
        std::vector<Desk> Desks;
        std::vector<Cabinet> Cabinets;
        CabinetFrame cabinetFrame;
        MaterialFrame materialFrame;
        ChatFrame chatFrame; 
        GameIcon Icons;
        Register reg;
        SDL_Color color = { 10, 10, 10, 255};

        Uint32 CurrentTime;
        Uint32 LastTime;

        Clock TestClock;
        const int HourTime = 10000;

        int TotalMoney = 0;
        bool isChatFrameShowing = 0;
        int TotalDessert = 0;
        int ChatDelayTime = 0;
        bool WhetherReadingProduct;
        bool isSettingNewProduct;
        bool CheckSetting;
        bool isSummaryShowing;
        bool isMaterialFrameShowing;

        int TotalCustomers;
        int ReadingPage;
        int CurrentCabnet;
        int currentalpha = 0;
        bool NightChanged;

        GameEvent TestEvent;
        SummaryFrame summaryFrame;
        CheckUpdate CheckEvent;
        UnlockFrame unlockFrame;
        SDL_Rect BackGroundRect = {0,0,800,600};

        enum TimeStage {day, night, trans};
        TimeStage CurrentStage = day;
        private:

};