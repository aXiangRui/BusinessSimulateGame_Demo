#pragma once

#include"RUI_Material.h"
#include"RUI_ResourceManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

class BaseMaterial : public Material
{
    public:
        BaseMaterial() = default;
        ~BaseMaterial() = default;

        void InitLevel(int materialid, int SweetNumber, int FullNumber, int TasteNumber,std::string name, std::string path)
        {
            SweetLevel = SweetNumber;
            FullLevel = FullNumber;
            TasteLevel = TasteNumber;
            Name = name;
            LoadPath = path;
            Kind = 0;
        }

        void SetRect(int mx, int my, int mw, int mh)
        {
            x = mx;
            y = my;
            w = mw;
            h = mh;
            Rect = {x,y,w,h};
        }

        void RenderMaterial(SDL_Renderer* Renderer)
        {
            MaterialTexture = ResourceManager::instance()->FindTexture(LoadPath.c_str());
            SDL_RenderCopy(Renderer, MaterialTexture, nullptr, &Rect);
        }

        int GetX()
        {
            return x;
        }

        int GetY()
        {
            return y;
        }

        int GetW()
        {
            return w;
        }

        int GetH()
        {
            return h;
        }

    private:
    int SweetLevel;
    int FullLevel;
    int TasteLevel;
    int MaterialID;
    int Kind;
    int x,y;
    int w,h;
    std::string LoadPath;
    std::string Name;
    SDL_Rect Rect;
    SDL_Texture* MaterialTexture;
};

class Plate
{
    public:
    Plate() = default;
    ~Plate() = default;

    void InitPlate(int mx,int my, int id)
    {
        x = mx;
        y = my;
        switch(id)
        {
            case 0:PlateTexture = ResourceManager::instance()->FindTexture("smallplate");break;
            case 1:PlateTexture = ResourceManager::instance()->FindTexture("mediumplate");break;
            case 2:PlateTexture = ResourceManager::instance()->FindTexture("largeplate");break;
            default:break;
        }   
        Rect = {x,y,256,256};
        LastTime = 0;
        WhetherAppear = 0;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        SDL_RenderCopy(Renderer, PlateTexture, nullptr, &Rect);
    }

    void AnimationRender(SDL_Renderer* Renderer, int CurrentTime)
    {
        if(CurrentTime - LastTime >= 500)
        {
            if(WhetherAppear == 0)
            {
                WhetherAppear = 1;
            }
            else
            {
                WhetherAppear = 0;
            }
            LastTime = CurrentTime;
        }
        if(WhetherAppear)
        {
            SDL_RenderCopy(Renderer, PlateTexture, nullptr, &Rect);  
        }
    }

    private:
    int x,y;
    int LastTime;
    bool WhetherAppear;
    SDL_Texture* PlateTexture;
    SDL_Rect Rect;

};