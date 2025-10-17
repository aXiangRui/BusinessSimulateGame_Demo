#pragma once

#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<sstream>
#include"RUI_Customer.h"

class CustomerManager
{
    public:
    CustomerManager() = default;
    ~CustomerManager() = default;

    std::vector<Customer> Customers;

    void InitCustomerManager()
    {
        if(hasloaded == 0)
        {
            std::ifstream File("./save/Customers.txt");
            std::string string;
            int line = 0;
            while(std::getline(File,string))
            {
                line++;
                std::istringstream iss(string);
                int cID;
                int pID;
                std::string cName;
                std::string cPath;
                int pre;
                iss>>cID>>pID>>cName>>cPath>>pre;
                SDL_Log("%d %d %s %s %d",cID,pID,cName.c_str(),cPath.c_str(),pre);
                Customer a;
                a.InitCustomer(cID,pID,cName,cPath,pre);
                SDL_Log("%d",a.GetHasJoined());
                Customers.push_back(a);
            }
        }
        hasloaded = 1;
    }

    int GetCustomerID(int i)
    {
       return Customers[i].GetCustomerID(); 
    }

    int GetPreferDessertID(int i)
    {
        return Customers[i].GetPreferDessertID();
    }

    std::string GetCustomerName(int i)
    {
        return Customers[i].GetCustomerName();
    }

    std::string GetCustomerPath(int i)
    {
        return Customers[i].GetCustomerPath();
    }

    int GetCustomerPreference(int i)
    {
        return Customers[i].GetCustomerPreference();
    }

    int GetCustomersSize()
    {
        return Customers.size();
    }

    private:
    bool hasloaded;
};