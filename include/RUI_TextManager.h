#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<string>

class TextManager
{
    public:
    TextManager() = default;
    ~TextManager() = default;

    std::vector<std::string>CustomerText;
    std::vector<std::string>StoreText;

    void Init()
    {
        std::ifstream File("./save/Text.txt");
        std::string string;
        int line = 0;
        while(std::getline(File,string))
        {
            line++;
            std::istringstream iss(string);

            if(string[0] == '#')
                continue;

            if(string == "助理")
            {
                std::string text;
                while(true)
                {
                    File >> text;
                    StoreText.push_back(text);
                    if(text == "end")
                        break;
                }
            }
            if(string == "顾客")
            {
                std::string text;
                while(true)
                {
                    File >> text;
                    CustomerText.push_back(text);
                    if(text == "end")
                        break;
                }
            }
        }
        
        StoreText.pop_back();
        CustomerText.pop_back();
        //删除数组末尾的end标识  
        // SDL_Log("%d %d",StoreText.size(), CustomerText.size());
        // for(int i = 0; i < StoreText.size(); i++)
        // {
        //     SDL_Log("%s",StoreText[i].c_str());
        // }              
        // SDL_Log("--------");
        // for(int i = 0; i < CustomerText.size(); i++)
        // {
        //     SDL_Log("%s",CustomerText[i].c_str());
        // }    
        File.close();
    }
};