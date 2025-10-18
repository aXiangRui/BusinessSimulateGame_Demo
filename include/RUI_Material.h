#pragma once

#include<string>
#include<SDL2/SDL.h>
#include"RUI_ResourceManager.h"

class Material
{
    public:
        Material() = default;
        ~Material() = default;

        // virtual void InitLevel(int materialid, int SweetNumber, int FullNumber, int TasteNumber, std::string name, std::string path){};
        void InitMaterial(int id, int type, std::string name, std::string Path,int sweet, int full, int taste)
        {
            MaterialID = id;
            MaterialType = type;
            Name = name;
            LoadPath = Path;
            SweetLevel = sweet;
            FullLevel = full;
            TasteLevel = taste;
            MaterialTexture = ResourceManager::instance()->FindTexture(LoadPath.c_str());
            x = 150; y = -100;
            w = 256; h = 256;
            Rect = {x,y,w,h};
            LastTime = 0;
        }

        void AddSweetNumber(int SweetNumber)
        {
            SweetLevel = SweetLevel + SweetNumber;
        }

        void AddFullNumber(int FullNumber)
        {
            FullLevel = FullLevel + FullNumber;
        }

        void AddTasteNumber(int TasteNumber)
        {
            TasteLevel = TasteLevel + TasteNumber;
        }

        int GetSweetNumber()
        {
            return SweetLevel;
        }

        int GetFullNumber()
        {
            return FullLevel;
        }

        int GetTasteNumber()
        {
            return TasteLevel;
        }

        void SetName(std::string name)
        {
            Name = name;
        }

        std::string GetName()
        {
            return Name;
        }

        std::string GetPath()
        {
            return LoadPath;
        }

        void SetRenderPosition(int mx, int my)
        {
            x = mx;
            y = my;
        }

        int GetXPosition()
        {
            return x;
        }

        int GetYPosition()
        {
            return y;
        }

        int GetID()
        {
            return MaterialID;
        }

        void moveUpdate(int CurrentTime, int i)
        {
            if(CurrentTime - LastTime >= 16 && y <= 120 - i * 20)
            {
                y = y + 5;
                Rect = {x,y,w,h};
                LastTime = CurrentTime;
            }
        }

        void onRender(SDL_Renderer* Renderer)
        {
            SDL_RenderCopy(Renderer, MaterialTexture, nullptr, &Rect);
        };

        void onRender(SDL_Renderer* Renderer,SDL_Rect newRect, int i)
        {
            newRect.y = newRect.y - i * 20;
            SDL_RenderCopy(Renderer, MaterialTexture, nullptr, &newRect);
        };
        
    private:
        int SweetLevel;
        int FullLevel;
        int TasteLevel;
        int MaterialID;
        int Kind;
        int x,y;
        int w,h;
        int MaterialType;
        std::string LoadPath;
        std::string Name;
        SDL_Texture* MaterialTexture;
        SDL_Rect Rect;
        int LastTime;
};