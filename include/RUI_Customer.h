#pragma once

#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include"RUI_ResourceManager.h"
#include"RUI_Chair.h"
#include"RUI_Cabinet.h"

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
            SitTime = 0;
            ChooseTime = 0;

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

        void SetSitTime(int time)
        {
            SitTime = time;
        }

        void SetChooseTime(int time)
        {
            ChooseTime = time;
        }

        int GetSitTime()
        {
            return SitTime;
        }

        int GetChooseTime()
        {
            return ChooseTime;
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

        void Update(std::vector<Chair>& Chairs,int currentTime, std::vector<Cabinet>& Cabtines)
        {
            switch(CurrentStage)
            {
                case CustomerStage::Enter:
                {
                    EnterStore(currentTime);
                    break;
                }
                case CustomerStage::Choose:
                {
                    ChooseDessert(Cabtines,currentTime);
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

        void EnterStore(int currentTime)
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
            else if(x < 200 && y >= 450)
            {
                y = y - speed;
            }
            if(x < 200 && y < 450)
            {
                SetChooseID(rand()%24);
                ChooseTime = currentTime;
                CurrentStage = CustomerStage::Choose;
            }
        }

        void ChooseDessert(std::vector<Cabinet>&Cabinets, int currentTime)
        {
            if(x < Cabinets[chooseID].GetX() + 32)
            {
                x = x + speed;
            }
            else if(x > Cabinets[chooseID].GetX() + 32)
            {
                x = x - speed;
            }
            else if(y < Cabinets[chooseID].GetY() - 16)
            {
                y = y + speed;
            }
            else if(y > Cabinets[chooseID].GetY() - 16)
            {
                y = y - speed;
            }
            else if(x == Cabinets[chooseID].GetX() + 32 && y == Cabinets[chooseID].GetY() - 16)
            {
                if(currentTime - ChooseTime >= 5000 + rand() % 500 - 250)
                {
                    CurrentStage = CustomerStage::Buy;
                }
            }
        }

        void Pay(int CurrentTime)
        {
            if(y >= 150)
            {
                y = y - speed;
            }
            else if(x <= 350)
            {
                toward = 1;
                x = x + speed;
            }
            if(x > 350 && y < 150)
            {
                SitTime = CurrentTime;
                CurrentStage = CustomerStage::Eat;
            }
        }

        void Eat(std::vector<Chair>& Chairs, int CurrentTime)
        {
            if(isEating == -1)
            {
                for(int i = 0; i < Chairs.size(); i++)
                {
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
                    if(x - Chairs[isEating].GetX() >= 0)
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
                    if(y - Chairs[isEating].GetY() >= 0)
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
                    
                    if(CurrentTime - SitTime >= 10000 + rand()% 5000 - 2500)
                    {    
                        Chairs[isEating].SetUsing(0);
                        CurrentStage = CustomerStage::Leave;
                    }
                }
            }
            //CurrentStage = CustomerStage::Leave;
        }

        void SetChooseID(int i)
        {
            chooseID = i;
        }

        void LeaveStore()
        {
            if(x <= 400)
            {
                x = x + speed;
            }
            else if(y <= 350)
            {
                y = y + speed;
            }
            else if(x <= 800)
            {
                toward = 1;
                x = x + speed;
            }
            if(x > 800 && y > 350 && CurrentStage == CustomerStage::Leave)
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

        int getX()
        {
            return x;
        }

        int getY()
        {
            return y;
        }

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int SitTime;
        int ChooseTime;
        CustomerStage CurrentStage;

        int chooseID;

        bool EnterFinish;
        bool ChooseFinish;
        bool BuyFinish;
        bool EatFinsih;
        bool QuitFinish;
        int isEating;
        bool toward;
        int speed = 2;
};