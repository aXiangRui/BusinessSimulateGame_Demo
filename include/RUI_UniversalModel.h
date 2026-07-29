#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "RUI_ResourceManager.h"

struct ModelData
{
    SDL_Texture* universalTexture = nullptr;
    SDL_Texture* coverTexture = nullptr;
    SDL_Rect Rect;
};

class RUI_UniversalModel
{
    public:
    RUI_UniversalModel() = default;
    ~RUI_UniversalModel() = default;

    void Init(std::string shapeName)
    {
        topModel.universalTexture = ResourceManager::instance()->FindTexture(("Top" + shapeName).c_str());
        for (int i = 0; i < 3; i++)
            sideModel[i].universalTexture = ResourceManager::instance()->FindTexture(("Side" + shapeName).c_str());
        topModel.Rect = {0, 0, 32, 32};
        for (int i = 0; i < 3; i++)
            sideModel[i].Rect = {0, 0, 32, 32};
        if (topModel.universalTexture == nullptr || sideModel[0].universalTexture == nullptr)
            throw std::runtime_error("RUI_UniversalModel Init Error: Texture Not Found");
    }

    void SetCoverTexture(std::string address)
    {
        topModel.coverTexture = ResourceManager::instance()->FindTexture(("Top" + address).c_str());
        for (int i = 0; i < 3; i++)
            sideModel[i].coverTexture = ResourceManager::instance()->FindTexture(("Side" + address).c_str());
        if (topModel.coverTexture == nullptr || sideModel[0].coverTexture == nullptr)
            throw std::runtime_error("RUI_UniversalModel SetCoverTexture Error: Texture Not Found");
    }

    void SetPosition(int x, int y)
    {
        topModel.Rect.x = x;
        topModel.Rect.y = y;
        for (int i = 0; i < 3; i++)
        {
            sideModel[i].Rect.x = x;
            sideModel[i].Rect.y = y;
        }
    }

    void SetSlice(int sliceCount)
    {
        currentSlice = sliceCount - 1; // 0=1片 1=2片 2=3片
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if (topModel.coverTexture)
        {
            SDL_RenderCopy(Renderer, topModel.coverTexture, nullptr, &topModel.Rect);
            SDL_SetTextureBlendMode(topModel.universalTexture, SDL_BLENDMODE_MUL);
            SDL_RenderCopy(Renderer, topModel.universalTexture, nullptr, &topModel.Rect);
            SDL_SetTextureBlendMode(topModel.universalTexture, SDL_BLENDMODE_NONE);
        }

        int si = currentSlice;
        if (si >= 0 && si < 3 && sideModel[si].coverTexture)
        {
            SDL_RenderCopy(Renderer, sideModel[si].coverTexture, nullptr, &sideModel[si].Rect);
            SDL_SetTextureBlendMode(sideModel[si].universalTexture, SDL_BLENDMODE_MUL);
            SDL_RenderCopy(Renderer, sideModel[si].universalTexture, nullptr, &sideModel[si].Rect);
            SDL_SetTextureBlendMode(sideModel[si].universalTexture, SDL_BLENDMODE_NONE);
        }
    }

    private:
    ModelData topModel;
    ModelData sideModel[3];
    int currentSlice = 0;
};

class UniversalModelManager
{
    public:
    UniversalModelManager() = default;
    ~UniversalModelManager() = default;

    void Init()
    {
        models.clear();
        for (int i = 0; i < 5; i++)
        {
            RUI_UniversalModel model;
            model.Init(shapeName[i]);
            models.push_back(model);
        }
    }

    RUI_UniversalModel& GetModel(std::string name)
    {
        for (int i = 0; i < (int)models.size(); i++)
        {
            if (shapeName[i] == name)
                return models[i];
        }
        throw std::runtime_error("UniversalModelManager GetModel Error: Model Not Found");
    }

    private:
    std::string shapeName[5] =
    {
        "Circle",
        "Square",
        "Triangle",
        "Heart",
        "Star",
    };
    std::vector<RUI_UniversalModel> models;
};
