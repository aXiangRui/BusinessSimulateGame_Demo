#pragma once

#include<string>
#include<SDL2/SDL.h>

class Material
{
    public:
        Material() = default;
        ~Material() = default;

        void InitLevel(int materialid, int SweetNumber, int FullNumber, int TasteNumber)
        {
            MaterialID = materialid;
            SweetLevel = SweetNumber;
            FullLevel = FullNumber;
            TasteLevel = TasteNumber;
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
        
    private:
        int SweetLevel;
        int FullLevel;
        int TasteLevel;
        int MaterialID;
        std::string Name;
};