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
            
            if(string[0] == '#')
                continue;

            int cID;
            int SweetNumber;
            int FullNumber;
            int TasteNumber;
            std::string dname;
            std::string dPath;
            bool whetherbase;
            iss>>cID>>SweetNumber>>FullNumber>>TasteNumber>>dname>>dPath>>whetherbase;
            SDL_Log("新甜品id:%d,甜度:%d,饱腹感:%d,味道:%d,名字:%s,路径:%s",
            cID,
            SweetNumber,
            FullNumber,
            TasteNumber,
            dname.c_str(),
            dPath.c_str()
        );
            Dessert a;
            a.InitLevel(cID,SweetNumber,FullNumber,TasteNumber,dname,dPath,whetherbase);
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

    void onRender(SDL_Renderer* Renderer,int i)
    {
        Desserts[i].onRender(Renderer);
    }

    bool GetWhetherBase(int i)
    {
        return Desserts[i].GetWhetherBase();
    }

    void onRender(SDL_Renderer* Renderer,int i, SDL_Rect newRect)
    {
        Desserts[i].onRender(Renderer,newRect);
    }

    void onRender(SDL_Renderer* Renderer,int i,int mx,int my)
    {
        Desserts[i].onRender(Renderer,mx,my);
    }

    int GetDessertsSize()
    {
        return Desserts.size();
    }

    void quit()
    {
        Desserts.clear();
    }
};