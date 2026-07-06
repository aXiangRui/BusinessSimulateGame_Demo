#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <queue>
#include "RUI_ResourceManager.h"
#include "RUI_Dessert.h"
#include "RUI_Cabinet.h"
#include "RUI_Chair.h"
#include "RUI_Customer.h"
#include "RUI_Tool.h"

class StoreServer
{
    public:
        StoreServer() = default;
        ~StoreServer() = default;

    enum class CurrentStage
    {
        waiting,
        paying,
        handing
    };
    void init()
    {
        x = 0 + rand() % 20;
        y = 200 + rand() % 100;
        toward = 0;
        Address = "default";
        ServerTexture = ResourceManager::instance()->FindTexture("default");
        DessertTexture = ResourceManager::instance()->FindTexture("smallcake");
        ServerRect = { x, y, 64, 64};
        Stage = CurrentStage::waiting;
        speed = 2;
        handingFreeTime = 0;
        handingNumber = 0;
        currentCabinet = -1;
        speed = rand() % 5 + 1;
    }

    int GetCurrentStage()
    {
        switch(Stage)
        {
            case CurrentStage::waiting:
                return 0;break;
            case CurrentStage::paying:
                return 1;break;
            case CurrentStage::handing:
                return 2;break;
            default:return -1;break;
        }
    }

    void SetCurrentStage(int n)
    {
        switch(n)
        {
            case 0:
            {
                Stage = CurrentStage::waiting;break;
            }
            case 1:
            {
                Stage = CurrentStage::paying;break;
            }
            case 2:
            {
                Stage = CurrentStage::handing;break;
            }
            default:break;
        }
    }

    void update(int CurrentTime, std::vector<SmallCake>& smallcakes, std::vector<Cabinet>& cabinets, Register& reg, int QueueNumber)
    {
        switch(Stage)
        {
            case CurrentStage::waiting:
            {
                Waiting(CurrentTime, smallcakes, cabinets, reg, QueueNumber);
                break;
            }
            case CurrentStage::paying:
            {
                Paying(reg, QueueNumber);
                break;
            }
            case CurrentStage::handing:
            {               
                Handing(CurrentTime, smallcakes, cabinets);
                break;
            }
            default:
            break;
        }
    }

    void Waiting(int CurrentTime, std::vector<SmallCake>& smallcakes, std::vector<Cabinet>& cabinets, Register& reg, int QueueNumber)
    {
        if( x < 250 )
        {
            x += speed;
            toward = 1;
        }    
        else if( y > 130 )
        {
            y -= speed;
        }   
        else if( y < 130)
        {
            y += TargetSpeedChangeY( 130, y , speed);
        }
        else if( x > 250)
        {
            x -= TargetSpeedChangeX( 250, x, speed);
            toward = 0;
        }
        if( QueueNumber != 0 && reg.GetIsPaying() == 0 )
        {
            Stage = CurrentStage::paying;
        }
        else if( handingNumber == 0 && smallcakes.size() != 0 )
        {
            Stage = CurrentStage::handing;
        }
    }

    void Handing(int CurrentTime, std::vector<SmallCake>& smallcakes, std::vector<Cabinet>& cabinets)
    {
        if(handingNumber == 0)
        {
            if(x > 100)
            {
                toward = 0;
                x -= speed;
            }
            else if(y > 130)
            {
                y -= speed;
            }
            else if(x < 100)
            {
                toward = 1;
                x += TargetSpeedChangeX(100, x, speed);
            }
            else if( y < 130)
            {
                y += TargetSpeedChangeY(130, y, speed);
            }
            if( x == 100 && y == 130)
            {
                if(smallcakes.size() == 0)
                {
                    Stage = CurrentStage::waiting;
                }
                if(handingFreeTime == 0)
                {
                    handingFreeTime = CurrentTime;
                }
                if( CurrentTime - handingFreeTime >= 1000)
                {
                    currentCabinet = smallcakes[0].GetID();
                    handingNumber = RemoveSmallCakesByID(smallcakes, smallcakes[0].GetID());
                    for(int i = 0; i < smallcakes.size(); i++)
                    {
                        smallcakes[i].updateID(i);
                    }
                    handingFreeTime = 0;
                }
            }
        }
        if( handingNumber != 0 && currentCabinet != -1)
        {
            if( x < cabinets[currentCabinet].GetX() + 32)
            {
                toward = 1;
                x += TargetSpeedChangeX(cabinets[currentCabinet].GetX() + 32, x, speed);
            }
            else if( x > cabinets[currentCabinet].GetX() + 32)
            {
                toward = 0;
                x -= speed;
            }
            else if( y < cabinets[currentCabinet].GetY() - 16)
            {
                y += TargetSpeedChangeY(cabinets[currentCabinet].GetY()-16, y, speed);
            }
            else if( y > cabinets[currentCabinet].GetY() - 16)
            {
                y -= speed;
            }
            if( x == cabinets[currentCabinet].GetX() + 32 && y == cabinets[currentCabinet].GetY() - 16)
            {
                toward = 0;
                if(handingFreeTime == 0)
                {
                    handingFreeTime = CurrentTime;
                }
                if(CurrentTime - handingFreeTime >= 1000 && handingNumber >= 0)
                {
                    cabinets[currentCabinet].AddDessertNumber(1);
                    handingFreeTime = 0;
                    SDL_Log("当前手中甜点数%d",handingNumber);
                    handingNumber--;
                }
                if( handingNumber == 0 )
                {
                    currentCabinet = -1;
                    handingFreeTime = 0;
                    Stage = CurrentStage::waiting;
                }
            }  
        }
    }

    void Paying(Register& reg, int QueueNumber)
    {
        if( y > 110)
        {
            y -= speed;
        }
        else if( y < 110)
        {
            y += TargetSpeedChangeY(110, y, speed);
        }
        else if( x < 350)
        {
            x += speed;
            toward = 1;
        }
        else if( x > 350)
        {
            x -= TargetSpeedChangeX(350, x, speed);
            toward = 0;
        }
        if( x == 350 && y == 110)
        {
            if(reg.GetIsPaying() == 0)
            {       
                reg.SetIsPaying(1);
            }
            // else
            // {
            //     Stage = CurrentStage::waiting;
            //     reg.SetIsPaying(0);
            // }
            if(QueueNumber == 0)
            {
                Stage = CurrentStage::waiting;
                reg.SetIsPaying(0);
            }
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        ServerRect = { x, y, 64, 64};
        if(toward == 0)
            SDL_RenderCopy(Renderer, ServerTexture, nullptr, &ServerRect);
        else
        {
            SDL_RenderCopyEx(Renderer, ServerTexture, nullptr, &ServerRect, 0, 0, SDL_FLIP_HORIZONTAL);
        } 
        for(int i = 0; i < handingNumber; i++)
        {
            if(toward == 0)
            {
                SDL_Rect cakeRect = { x - 16, y - i * 8 - 16, 64, 64};
                SDL_RenderCopy(Renderer, DessertTexture, nullptr, &cakeRect);
            }
            if(toward == 1)
            {
                SDL_Rect cakeRect = { x + 16, y - i * 8 - 16, 64, 64};
                SDL_RenderCopy(Renderer, DessertTexture, nullptr, &cakeRect);
            }
        }   
    }
    void SetPosition(int mx, int my)
    {
        x = mx; y = my;
    }
    void SetSpeed(int s)
    {
        speed = s;
    }
    std::string GetAddress()
    {
        return Address;
    }
    int GetX()
    {
        return x;
    }
    int GetY()
    {
        return y;
    }
    int GetSpeed()
    {
        return speed;
    }
    private:
    int x,y;
    int toward;
    std::string Address;
    SDL_Texture* ServerTexture;
    SDL_Texture* DessertTexture;
    SDL_Rect ServerRect;
    CurrentStage Stage;
    int speed;
    int handingFreeTime;
    int handingNumber;
    int currentCabinet;
};