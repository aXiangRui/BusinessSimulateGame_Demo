#pragma once

#include<SDL2/SDL.h>
#include<string>

class Customer
{
    public:
        Customer() = default;
        ~Customer() = default;


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

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int EnterTime;
        int QuitTime;
};