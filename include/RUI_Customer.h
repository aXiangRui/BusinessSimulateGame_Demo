#pragma once

#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include"RUI_ResourceManager.h"

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
            x = 300;
            y = 300;
            EnterTime = 0;
            QuitTime = 0;
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
            SDL_Rect Rect = {x+(CustomerID%5)*20,y+(CustomerID/5)*20,64,64};
            if(SDL_RenderCopy(Renderer, NormalTexture, nullptr, &Rect) != 0)
            {
                SDL_Log("OnRender: SDL_RenderCopy failed for id=%d name=%s error=%s", CustomerID, CustomerName.c_str(), SDL_GetError());
            }
        }

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int EnterTime;
        int QuitTime;
};