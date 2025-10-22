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

                if(string[0] == '#')
                    continue;

                int cID;
                int pID;
                std::string cName;
                std::string cPath;
                int pre;
                int app;
                iss>>cID>>pID>>cName>>cPath>>app>>pre;
                SDL_Log("%d %d %s %s %d",cID,pID,cName.c_str(),cPath.c_str(),app);
                Customer a;
                a.InitCustomer(cID,pID,cName,cPath,app,pre);
                // SDL_Log("%d",a.GetHasJoined());
                Customers.push_back(a);
            }
        }
        hasloaded = 1;
    }

    void update()
    {
        Customers.clear();
        std::ifstream File("./save/Customers.txt");
        std::string string;
        int line = 0;
        while(std::getline(File,string))
        {
            line++;
            std::istringstream iss(string);

            if(string[0] == '#')
                continue;

            int cID;
            int pID;
            std::string cName;
            std::string cPath;
            int pre;
            int app;
            iss>>cID>>pID>>cName>>cPath>>app>>pre;
            SDL_Log("%d %d %s %s %d",cID,pID,cName.c_str(),cPath.c_str(),app);
            Customer a;
            a.InitCustomer(cID,pID,cName,cPath,app,pre);
            SDL_Log("%d",a.GetHasJoined());
            Customers.push_back(a);
        }
    }

    void Save()
    {
        std::ofstream file("./save/Customers.txt");
        file << "#以下数据依次为ID，喜欢的甜点ID,顾客名字,顾客地址,是否已解锁(1表示已解锁,0表示未解锁),好感度"<<std::endl;
        for(int i = 0; i < Customers.size(); i++)
        {
            file << Customers[i].GetCustomerID() << " ";
            file << Customers[i].GetPreferDessertID() << " ";
            file << Customers[i].GetCustomerName() << " ";
            file << Customers[i].GetCustomerPath() << " ";
            file << Customers[i].GetWhetherAppear() << " ";
            file << Customers[i].GetCustomerPreference() << " ";
            file << std::endl;
        }
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

    bool GetWhetherAppear(int i)
    {
        return Customers[i].GetWhetherAppear();
    }

    private:
    bool hasloaded;
};