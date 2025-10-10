#pragma once

#include<string>
#include<SDL2/SDL.h>

class Material
{
    public:
        Material() = default;
        ~Material() = default;

        SDL_Texture* MaterialTexture;

        void InitLevel(int materialid, int SweetNumber, int FullNumber, int TasteNumber)
        {
            MaterialID = materialid;
            SweetLevel = SweetNumber;
            FullLevel = FullNumber;
            TasteLevel = TasteNumber;
        }

        void SetLoadPath(std::string path)
        {
            LoadPath = path;
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

        virtual void RenderMaterial(SDL_Renderer* Renderer){};
        
    private:
        int SweetLevel;
        int FullLevel;
        int TasteLevel;
        int MaterialID;
        int x,y;
        std::string LoadPath;
        std::string Name;
};