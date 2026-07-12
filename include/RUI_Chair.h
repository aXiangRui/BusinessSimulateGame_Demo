#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"
#include"RUI_Furniture.h"

class Chair
{
    public:
    Chair() = default;
    ~Chair() = default;

    void InitChair(int id)
    {
        ChairID = id;
        x = (ChairID / 8) * 200 + 450 + 80 * (ChairID % 2);
        y = (ChairID / 2 % 4) * 100;
        if(ChairID % 2 == 0)
        {
            toward = 0;
        }
        else
        {
            toward = 1;
        }
        isUsing = 0;
    }

    /// 使用像素坐标初始化（取代公式），自动吸附到格点
    void InitChair(int id, int pixelX, int pixelY, bool flipped = false)
    {
        ChairID = id;
        SetPosition(pixelX, pixelY);
        toward = flipped ? 1 : 0;
        isUsing = 0;
    }

    void InitChair(int id, GridPos pos)
    {
        InitChair(id);
        SetPosition(pos.col * Furniture::FurnitureWidth + Furniture::offsetX, pos.row * Furniture::FurnitureHeight + Furniture::offsetY);
    }

    void SetPosition(int mx, int my)
    {
        // 吸附到最近的格点（含 Furniture 偏移）
        x = ((mx - Furniture::offsetX + Furniture::FurnitureWidth / 2) / Furniture::FurnitureWidth) * Furniture::FurnitureWidth + Furniture::offsetX;
        y = ((my - Furniture::offsetY + Furniture::FurnitureHeight / 2) / Furniture::FurnitureHeight) * Furniture::FurnitureHeight + Furniture::offsetY;
    }

    void SetUsing(bool x)
    {
        isUsing = x;
    }

    bool GetUsing()
    {
        return isUsing;
    }

    int GetChairID()
    {
        return ChairID;
    }

    static const int RENDER_SIZE = 64;
    static const int GRID_SIZE  = 32;
    static const int RENDER_OFFSET = (RENDER_SIZE - GRID_SIZE) / 2;  // 8

    int GetX() { return x; }
    int GetY() { return y; }
    int GetRenderX() { return x - RENDER_OFFSET; }
    int GetRenderY() { return y - (int)(RENDER_OFFSET * 2); }

    void onRender(SDL_Renderer* Renderer)
    {
        if(!ChairTexture)
            ChairTexture = ResourceManager::instance()->FindTexture("chair");
        // 48×48 居中于 32×32 格点
        SDL_Rect Rect = {x - RENDER_OFFSET, y - RENDER_OFFSET * 2, RENDER_SIZE, RENDER_SIZE};
        if(toward == 0)
        {
            SDL_RenderCopyEx(Renderer,ChairTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
        }
        else
        {
            SDL_RenderCopy(Renderer,ChairTexture,nullptr,&Rect);
        }
    }

    private:

    int ChairID;
    int x,y;
    bool isUsing;
    bool toward;
    SDL_Texture* ChairTexture = nullptr;

};

class Desk
{
    public:
    Desk() = default;
    ~Desk() = default;

    void initDesk(int id)
    {
        DeskID = id;
        x = (DeskID / 4)* 200 + 490;
        y = (DeskID % 4) * 100;
    }

    /// 使用像素坐标初始化（取代公式）
    void initDesk(int id, int pixelX, int pixelY)
    {
        DeskID = id;
        SetPosition(pixelX, pixelY);
    }

    void initDesk(int id, GridPos pos)
    {
        DeskID = id;
        SetPosition(pos.col * Furniture::FurnitureWidth + Furniture::offsetX, pos.row * Furniture::FurnitureHeight + Furniture::offsetY);
    }

    void SetPosition(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    static const int RENDER_SIZE = 64;
    static const int GRID_SIZE  = 32;
    static const int RENDER_OFFSET = (RENDER_SIZE - GRID_SIZE) / 2;  // 8

    int GetX() { return x; }
    int GetY() { return y; }
    int GetRenderX() { return x - RENDER_OFFSET; }
    int GetRenderY() { return y - RENDER_OFFSET; }
    int GetDeskID() { return DeskID; }

    void onRender(SDL_Renderer* Renderer)
    {
        if(!DeskTexture)
        {
            DeskTexture = ResourceManager::instance()->FindTexture("desk");
        }
        // 48×48 居中于 32×32 格点
        SDL_Rect Rect = {x - RENDER_OFFSET, y - RENDER_OFFSET*2, RENDER_SIZE, RENDER_SIZE};
        SDL_RenderCopy(Renderer, DeskTexture, nullptr, &Rect);
    }

    private:
    int DeskID;
    int x,y;
    SDL_Texture* DeskTexture = nullptr;
};

class Register
{
    public:
    Register() = default;
    ~Register() = default;

    void InitRegister()
    {
        x = 350;
        y = 130;
        RegisterTexture = ResourceManager::instance()->FindTexture("register");
    }

    void onRender(SDL_Renderer* Renderer)
    {
        SDL_Rect Rect = {x,y,64,64};
        SDL_RenderCopy(Renderer,RegisterTexture,nullptr,&Rect);
    }

    void SetIsPaying(bool a)
    {
        isPaying = a;
    }

    bool GetIsPaying()
    {
        return isPaying;
    }

    int GetX() const { return x; }
    int GetY() const { return y; }

    private:
    SDL_Texture* RegisterTexture;
    int x,y;
    bool isPaying;
};