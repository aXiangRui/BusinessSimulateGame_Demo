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

    private:
        std::string CustomerName;
        int CustomerID;
        int PreferDessertID;
};