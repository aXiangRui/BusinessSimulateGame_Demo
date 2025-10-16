#pragma once

#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<sstream>
#include"RUI_Dessert.h"

class DessertManager
{
    public:
    DessertManager() = default;
    ~DessertManager() = default;

    std::vector<Dessert> Desserts;

    void InitDessertManager()
    {
        std::ifstream File("./save/Desserts.txt");
        std::string string;
        int line = 0;
        while(std::getline(File,string))
        {
            line++;
            std::istringstream iss(string);
            int cID;
            int SweetNumber;
            int FullNumber;
            int TasteNumber;
            std::string dname;
            std::string dPath;
            int pre;
            iss>>cID>>SweetNumber>>FullNumber>>TasteNumber>>dname>>dPath;
            SDL_Log("新甜品id:%d,甜度:%d,饱腹感:%d,味道:%d,名字:%s,路径:%s",
            cID,
            SweetNumber,
            FullNumber,
            TasteNumber,
            dname.c_str(),
            dPath.c_str()
        );
            Dessert a;
            a.InitLevel(cID,SweetNumber,FullNumber,TasteNumber,dname,dPath);
            Desserts.push_back(a);
        }
    }

    int GetDessertPrice(int i)
    {
        return Desserts[i].GetDessertPrice();
    }

    int GetDessertID(int i)
    {
       return Desserts[i].GetDessertID(); 
    }

    std::string GetDessertName(int i)
    {
        return Desserts[i].GetDessertName();
    }

    std::string GetDessertPath(int i)
    {
        return Desserts[i].GetDessertPath();
    }

    int GetDessertsSize()
    {
        return Desserts.size();
    }
};