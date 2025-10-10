#pragma once

#include<SDL2/SDL.h>
#include<string>

class Dessert
{
    public:
       Dessert() = default;
       ~Dessert() = default;
    SDL_Texture* DessertTexture;

    void InitLevel(int dessertid, int SweetNumber, int FullNumber, int TasteNumber)
    {
        DessertID = dessertid;
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
    int DessertID;
    std::string Name;
};