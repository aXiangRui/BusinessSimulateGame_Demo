#pragma once

#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include"RUI_ResourceManager.h"


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
            if(SDL_RenderCopy(Renderer, NormalTexture, nullptr, &Rect) != 0)
            {
                SDL_Log("OnRender: SDL_RenderCopy failed for id=%d name=%s error=%s", CustomerID, CustomerName.c_str(), SDL_GetError());
            }
        }

        void Update()
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
                    Pay();
                    break;
                }
                case CustomerStage::Eat:
                {
                    Eat();
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
                x = x - 5;
            }
            else if(x < 450 && y <= 500 && x >=200 )
            {
                y = y + 5;
            }
            else if(x >= 200 && x < 450 && y > 500)
            {
                x = x - 5;
            }
            else if(x < 200 && y >= 400)
            {
                y = y - 5;
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
                        x = x - 5;
                    }
                    if(x < 100 && y >= 150)
                    {
                        y = y - 5;
                    }
                    if(x < 100 && y < 150)
                    {
                        CurrentStage = CustomerStage::Buy;
                    }
                    break;
                }
            }
        }

        void Pay()
        {
            if(x <= 400)
            {
                x = x + 5;
            }
            else if(y >= 150)
            {
                y = y - 5;
            }
            if(x > 400 && y < 150)
            {
                CurrentStage = CustomerStage::Eat;
            }
        }

        void Eat()
        {
            CurrentStage = CustomerStage::Leave;
        }

        void LeaveStore()
        {
            if(y <= 350)
            {
                y = y + 5;
            }
            else if(x <= 800)
            {
                x = x + 5;
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
};