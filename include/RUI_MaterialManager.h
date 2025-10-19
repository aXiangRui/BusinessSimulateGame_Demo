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
                iss>>Kind>>MID>>type>>Name>>Path>>sweetlevel>>fulllevel>>tastelevel;
                
                Material a;
                a.InitMaterial(MID,type,Name,Path,sweetlevel,fulllevel,tastelevel);
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

    void quit()
    {
        DecorationMaterial.clear();
        BaseMaterial.clear();
    }
    private:

};