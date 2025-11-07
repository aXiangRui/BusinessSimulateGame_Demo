#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<string>
#include"RUI_ResourceManager.h"

class RUI_Icon
{
    public:
    RUI_Icon() = default;
    ~RUI_Icon() = default;
    
    // virtual void IconHoverRender(SDL_Renderer* Renderer,std::string address);
    // virtual void IconClickRender(SDL_Renderer* Renderer,std::string address);

    void InitIcon(int mx, int my, int mw, int mh, int i, std::string add)
    {
        x = mx;
        y = my;
        w = mw;
        h = mh;
        Rect = {x,y,w,h};
        id = i;
        address = add;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        IconTexture = ResourceManager::instance()->FindTexture(address.c_str());
        SDL_RenderCopy(Renderer,IconTexture,nullptr,&Rect);
    }

    void onRender(SDL_Renderer* Renderer, int i)
    {
        if(i == 1)
        {
            IconTexture = ResourceManager::instance()->FindTexture(address.c_str());
            SDL_RenderCopyEx(Renderer,IconTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
        }
    }

    bool isHovered(int mx, int my)
    {
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    bool isClicked(int mx, int my)
    {
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    private:
    int x,y,w,h;
    SDL_Rect Rect;
    int id;
    std::string address;
    SDL_Texture* IconTexture;
};

class GameIcon
{
    public:
    GameIcon() = default;
    ~GameIcon() = default;

    void Init()
    {
        RUI_Icon testicon;
        testicon.InitIcon(10,10,50,50,0,"sun");
        Icons.push_back(testicon);

        RUI_Icon CookingIcon;
        CookingIcon.InitIcon(740,470,50,50,1,"cooking");
        Icons.push_back(CookingIcon);

        RUI_Icon exiticon;
        exiticon.InitIcon(740,530,50,50,2,"exiticon");
        Icons.push_back(exiticon);

        RUI_Icon readicon;
        readicon.InitIcon(740,410,50,50,3,"readicon");
        Icons.push_back(readicon);

        RUI_Icon nexticon;
        nexticon.InitIcon(675,250,50,50,4,"nexticon");
        Icons.push_back(nexticon);

        RUI_Icon lasticon;
        lasticon.InitIcon(75,250,50,50,5,"nexticon");
        Icons.push_back(lasticon);

        RUI_Icon AddCabinetIcon;
        AddCabinetIcon.InitIcon(680,410,50,50,6,"defaulticon");
        Icons.push_back(AddCabinetIcon);
    }

    void onRender(SDL_Renderer* Renderer, bool isReading)
    {
        for(int i = 0; i < Icons.size(); i++)
        {      
            if(i != 4 && i != 5)
                Icons[i].onRender(Renderer);
            else
            {
                if(isReading)
                {
                    if(i == 4){Icons[i].onRender(Renderer);}
                    if(i == 5){Icons[i].onRender(Renderer,1);} 
                }
            }  
        }
    }

    void Quit()
    {
        Icons.clear();
    }

    std::vector<RUI_Icon>Icons;
    private:
};