#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include"RUI_Product.h"
#include"RUI_DessertManager.h"
#include"RUI_MaterialManager.h"
#include"RUI_BaseMaterial.h"

class ProductManager
{
    public:
    ProductManager() = default;
    ~ProductManager() = default;

    std::vector<ProducedProduct> products;

    void InitProductManager()
    {
        dManager.InitDessertManager();
        mManager.InitMaterialManager();
        plates.resize(3);
        for(int i = 0; i < 3; i++)
        {
            plates[i].InitPlate(200,200,i);
        }
        std::ifstream File("./save/Product.txt");
        std::string string;
        int line = 0;
        while(std::getline(File,string))
        {
            line++;
            std::istringstream iss(string);
            int ID;
            int BID;
            int d1,d2,d3;
            std::vector<int> did;
            int size;
            iss>>ID>>BID>>d1>>d2>>d3>>size;
            did.push_back(d1);
            did.push_back(d2);
            did.push_back(d3);

            SDL_Log("读取到%d号产品",ID);
            ProducedProduct a;
            a.LoadInit(ID,BID,did,size);
            products.push_back(a);
        }
    }

    std::string GetProductName(int i)
    {
        return products[i].GetProductName(dManager);
    }

    int GetProductPrice(int i)
    {
    //     return dManager.GetDessertPrice(i);
        return products[i].GetProductPrice(dManager,mManager);
    }

    void onRender(SDL_Renderer* Renderer, int i)
    {
        products[i].onRender(Renderer,dManager,mManager,plates,
        200,200,200,200);
    }

    void quit()
    {
        products.clear();
    }

    private:
    std::vector<Plate> plates;
    DessertManager dManager;
    MaterialManager mManager;
};