#pragma once

#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include"RUI_ResourceManager.h"
#include"RUI_Chair.h"

enum class CustomerStage
{
    Enter,
    Choose,
    Buy,
    Eat,
    Leave
};

class Customer
{
    public:
        Customer() = default;
        ~Customer() = default;
        std::vector<SDL_Texture*>CustomerTexture;
        SDL_Texture* NormalTexture = nullptr;

        void InitCustomer(int id, int preferid, std::string name)
        {
            CustomerID = id;
            PreferDessertID = preferid;
            CustomerName = name;
            x = 800;
            y = 450;
            EnterTime = 0;
            QuitTime = 0;

            CurrentStage = CustomerStage::Enter;
            EnterFinish = 0;
            ChooseFinish = 0;
            BuyFinish = 0;
            EatFinsih = 0;
            QuitFinish = 0;
            toward = 0;
            isEating = -1;
        }

        int GetCustomerID()
        {
            return CustomerID;
        }
        
        int GetPreferDessertID()
        {
            return PreferDessertID;
        }

        std::string GetCustomerName()
        {
            return CustomerName;
        }

        void SetEnterTime(int time)
        {
            EnterTime = time;
        }

        void SetQuitTime(int time)
        {
            QuitTime = time;
        }

        int GetEnterTime()
        {
            return EnterTime;
        }

        int GetQuitTime()
        {
            return QuitTime;
        }

        void loadTexture(std::string path)
        {
            CustomerTexture.push_back(ResourceManager::instance()->FindTexture(path));
        }

        void OnRender(SDL_Renderer* Renderer)
        {
            if(!Renderer)
            {
                SDL_Log("OnRender: Renderer is null for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            NormalTexture = ResourceManager::instance()->FindTexture(CustomerName.c_str());
            if(!NormalTexture)
            {
                SDL_Log("OnRender: texture not found for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            SDL_Rect Rect = {x,y,64,64};
            if(toward == 0)
            {
                SDL_RenderCopy(Renderer,NormalTexture,nullptr,&Rect);
            }
            else
            {
                SDL_RenderCopyEx(Renderer,NormalTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
            }
            
        }

        void Update(std::vector<Chair>& Chairs,int currentTime)
        {
            switch(CurrentStage)
            {
                case CustomerStage::Enter:
                {
                    EnterStore();
                    break;
                }
                case CustomerStage::Choose:
                {
                    ChooseDessert();
                    break;
                }
                case CustomerStage::Buy:
                {
                    Pay(currentTime);
                    break;
                }
                case CustomerStage::Eat:
                {
                    Eat(Chairs, currentTime);
                    break;
                }
                case CustomerStage::Leave:
                {
                    LeaveStore();
                    break;
                }
            }
        }

        void EnterStore()
        {
            if(x >= 450)
            {
                toward = 0;
                x = x - speed;
            }
            else if(x < 450 && y <= 500 && x >=200 )
            {
                y = y + speed;
            }
            else if(x >= 200 && x < 450 && y > 500)
            {
                toward = 0;
                x = x - speed;
            }
            else if(x < 200 && y >= 400)
            {
                y = y - speed;
            }
            if(x < 200 && y < 400)
            {
                CurrentStage = CustomerStage::Choose;
            }
        }

        void ChooseDessert()
        {
            switch(1)
            {
                case 1:
                case 2:
                default:
                {
                    if(x >= 100)
                    {
                        toward = 0;
                        x = x - speed;
                    }
                    if(x < 100 && y >= 150)
                    {
                        y = y - speed;
                    }
                    if(x < 100 && y < 150)
                    {
                        CurrentStage = CustomerStage::Buy;
                    }
                    break;
                }
            }
        }

        void Pay(int CurrentTime)
        {
            if(x <= 400)
            {
                toward = 1;
                x = x + speed;
            }
            else if(y >= 150)
            {
                y = y - speed;
            }
            if(x > 400 && y < 150)
            {
                EnterTime = CurrentTime;
                CurrentStage = CustomerStage::Eat;
            }
        }

        void Eat(std::vector<Chair>& Chairs, int CurrentTime)
        {
            if(isEating == -1)
            {
                for(int i = 0; i < Chairs.size(); i++)
                {
                    SDL_Log("%d",Chairs[i].GetUsing());
                    if(isEating != -1)
                        break;
                    else
                    {
                        if(Chairs[i].GetUsing() == 0)
                        {
                            Chairs[i].SetUsing(1);
                            isEating = i;
                        }
                    } 
                }             
                if(isEating == -1)
                {
                    CurrentStage = CustomerStage::Leave;
                }
            }
            else
            {
                if( x != Chairs[isEating].GetX())
                {
                    if(x - Chairs[isEating].GetX() > 0)
                    {
                        x = x - speed;
                        toward = 0;
                    }
                    else
                    {
                        x = x + speed;
                        toward = 1;
                    }
                }
                else if( y != Chairs[isEating].GetY())
                {
                    if(y - Chairs[isEating].GetY() > 0)
                    {
                        y = y - speed;
                    }
                    else
                    {
                        y = y + speed;
                    }        
                }
                if(x == Chairs[isEating].GetX() && y == Chairs[isEating].GetY())
                {
                    if(isEating % 2 == 0)
                    {
                        toward = 1;
                    }
                    else
                    {
                        toward = 0;
                    }
                    
                    if(CurrentTime - EnterTime >= 10000 + rand()% 2000 - 1000)
                    { 
                        CurrentStage = CustomerStage::Leave;
                        Chairs[isEating].SetUsing(0);
                    }
                }
            }
            //CurrentStage = CustomerStage::Leave;
        }

        void LeaveStore()
        {
            if(y <= 350)
            {
                y = y + speed;
            }
            else if(x <= 800)
            {
                toward = 1;
                x = x + speed;
            }
            if(x > 800)
            {
                QuitFinish = 1;
            }
        }

        bool GetQuit()
        {
            return QuitFinish;
        }

        bool GetToward()
        {
            return toward;
        }

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int EnterTime;
        int QuitTime;
        CustomerStage CurrentStage;

        bool EnterFinish;
        bool ChooseFinish;
        bool BuyFinish;
        bool EatFinsih;
        bool QuitFinish;
        int isEating;
        bool toward;
        int speed = 10;
};