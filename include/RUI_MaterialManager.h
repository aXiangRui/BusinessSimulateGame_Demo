#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include"RUI_Material.h"

class MaterialManager
{
    public:
    MaterialManager() = default;
    ~MaterialManager() = default;

    std::vector<Material> BaseMaterial;
    std::vector<Material> DecorationMaterial;

    void InitMaterialManager()
    {
        if(1)
        {
            std::ifstream File("./save/Material.txt");
            std::string string;
            int line = 0;
            while(std::getline(File,string))
            {
                line++;
                std::istringstream iss(string);

                if(string[0] == '#')
                    continue;

                int Kind;
                int MID;
                int type;
                std::string Name;
                std::string Path;
                int sweetlevel;
                int fulllevel;
                int tastelevel;
                bool unlock;
                iss>>Kind>>MID>>type>>Name>>Path>>sweetlevel>>fulllevel>>tastelevel>>unlock;
                
                Material a;
                a.InitMaterial(MID,type,Name,Path,sweetlevel,fulllevel,tastelevel,unlock);
                SDL_Log("读取到材料%s",Name.c_str());
                if(Kind == 0)
                {
                    BaseMaterial.push_back(a);
                }
                else
                {
                    DecorationMaterial.push_back(a);
                }
                
            }
        }
    }

    void Save()
    {
        std::ofstream File("./save/Material.txt");
        File << "以下数据依次为种类(1为装饰,0为基类),ID,食材种类,名字,储存地址,甜度,饱腹感,口感，是否解锁" << std::endl;
        File << "#食材分类如下:" << std::endl;
        File << "#---0---奶油类" << std::endl;
        File << "#---1---草莓类" << std::endl;
        File << "#---2---巧克力类" << std::endl;
        File << "#---3---水果类" << std::endl;
        File << "#---4---咖啡类" << std::endl;
        File << "#---5---饼干类" << std::endl;
        File << "#---6---果汁类" << std::endl;
        for(int i = 0; i < DecorationMaterial.size(); i++)
        {
            File << 1 << " ";
            File << DecorationMaterial[i].GetID() << " ";
            File << DecorationMaterial[i].GetMaterialType() << " ";
            File << DecorationMaterial[i].GetName() << " ";
            File << DecorationMaterial[i].GetPath() << " ";
            File << DecorationMaterial[i].GetSweetNumber() << " ";
            File << DecorationMaterial[i].GetFullNumber() << " ";
            File << DecorationMaterial[i].GetTasteNumber() << " ";
            File << DecorationMaterial[i].GetWhetherUnlock() << " ";
            File << std::endl;
        }
    }

    int GetDecorationSize()
    {
        return DecorationMaterial.size();
    }

    std::string GetDecorationName(int i)
    {
        return DecorationMaterial[i].GetName();
    }

    int GetDecorationID(int i)
    {
        return DecorationMaterial[i].GetID();
    }

    int GetDecorationType(int i)
    {
        return DecorationMaterial[i].GetMaterialType();
    }

    void quit()
    {
        DecorationMaterial.clear();
        BaseMaterial.clear();
    }
    private:

};