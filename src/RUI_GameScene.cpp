#include "../include/RUI_GameScene.h"

extern int WindowWidth;
extern int WindowHeight;

void RUI_GameScene::onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
{  
    TestEvent.input(event);
    switch(event.type)
    {               
        case SDL_MOUSEBUTTONDOWN:
        {            
            int mx = event.button.x;
            int my = event.button.y;
            if(CheckSetting)
            {
                // 一步计算偏移量，无需分支判断
                if (mx >= 200 && mx <= 600 && my >= 0 && my <= 600) {
                    int offset = ((mx > 400) ? 3 : 0) + (my / 200);
                    Cabinets[CurrentCabnet].SetDessertID(ReadingPage * 6 + offset);
                    CheckSetting = 0;
                    isSettingNewProduct = 1;
                }
            }
            if( isChatFrameShowing )
            {
                if(mx >= 200 && mx <= 600)
                {
                    isChatFrameShowing = 0;
                    ChatDelayTime = 0;
                }
            }
            if( isSummaryShowing )
            {
                if( mx >= 200 && mx <= 600)
                {
                    isSummaryShowing = 0;
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
            for(int i = 0; i < Icons.Icons.size(); i++)
            {
                if(Icons.Icons[i].isClicked(mx,my))
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
                                if( isMaterialFrameShowing == 1)
                                {
                                    isMaterialFrameShowing = 0;
                                }
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
                            break;
                        }
                        case 6:
                        {
                            if(Cabinets.size() < 24)
                            {
                                Cabinet a;
                                a.InitCabinet(Cabinets.size(),0,0);
                                Cabinets.push_back(a);
                                TotalMoney = TotalMoney - 1000 * Cabinets.size() -1000;
                            }
                            bool a = 0;
                            // if( isMaterialFrameShowing == 1 && a == 0)
                            // {
                            //     isMaterialFrameShowing = 0;
                            //     a = 1;
                            // }
                            // if( isMaterialFrameShowing == 0  & a == 0)
                            // {
                            //     materialFrame.SetTitle("奶油", "cream", textManager.MaterialText[0]);
                            //     isMaterialFrameShowing = 1;
                            // }
                            break;
                        }
                        case 7:
                        {
                            for(int i = 0; i < 5; i++)
                            {
                                SDL_Log("%s %d",customerManager.Customers[i].GetCustomerName().c_str(),customerManager.Customers[i].GetHasJoined());
                            }
                            break;
                        }
                        default:
                        break;
                    }
                }
            }
            for(int i = 0; i < Cabinets.size(); i++)
            {
                if(isSettingNewProduct == 0 && TestEvent.GetWhetherRenderCustomerFrame() == 0)
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
            for(int i = 0; i < Icons.Icons.size(); i++)
            {
                if(Icons.Icons[i].isHovered(mx,my))
                {
                    if(i <= 3 || i >= 6)
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
}