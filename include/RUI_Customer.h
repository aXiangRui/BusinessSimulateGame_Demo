#pragma once

#include<SDL2/SDL.h>
#include<string>
#include"RUI_ResourceManager.h"

class Customer
{
    public:
        Customer() = default;
        ~Customer() = default;
        std::vector<SDL_Texture*>CustomerTexture;

        void InitCustomer(int id, int preferid, std::string name)
        {
            CustomerID = id;
            PreferDessertID = preferid;
            CustomerName = name;
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
            
        }

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int EnterTime;
        int QuitTime;
};