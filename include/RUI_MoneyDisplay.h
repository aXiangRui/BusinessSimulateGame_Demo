#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

// 金额显示组件 —— GPU 纹理缓存，只在金额变化时重建
class MoneyDisplay
{
public:
    MoneyDisplay() = default;
    ~MoneyDisplay() = default;

    void Init(int posX, int posY, int fontSize = 36)
    {
        x = posX;
        y = posY;
        font = TTF_OpenFont("./resources/font/namidiansong.ttf", fontSize);
    }

    void SetMoney(int money)
    {
        if (money == cachedMoney && texture)
            return; // 没变，跳过

        cachedMoney = money;

        // 销毁旧纹理
        if (texture)
            SDL_DestroyTexture(texture);

        // 颜色：负数红
        SDL_Color color = (money < 0)
            ? SDL_Color{200, 40, 40, 255}
            : SDL_Color{10, 10, 10, 255};

        // 渲染文字 → surface → texture
        std::string text = "总金额" + std::to_string(money);
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        rect = {x, y, surf->w, surf->h};
        SDL_FreeSurface(surf);
    }

    void onRender(SDL_Renderer* r)
    {
        // 每次渲染记录 renderer，用于首次或重建纹理
        renderer = r;
        if (texture)
            SDL_RenderCopy(r, texture, nullptr, &rect);
    }

    void Quit()
    {
        if (texture)
            SDL_DestroyTexture(texture);
        if (font)
            TTF_CloseFont(font);
        texture = nullptr;
        font = nullptr;
    }

private:
    int x = 0, y = 0;
    int cachedMoney = INT_MIN;
    TTF_Font* font = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Rect rect = {};
    SDL_Renderer* renderer = nullptr;  // 用于首次 SetMoney 时创建纹理
};

