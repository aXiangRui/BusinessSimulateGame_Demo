#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "RUI_ResourceManager.h"

enum class FurnitureType
{
    None = 000,
    Cabinet = 101,
    Register = 102,
    Table = 201,
    Chair = 202,
};

enum class PlacementType
{
    Kitchen = 001,
    Showing = 002,
    Eating = 003,
};

struct GridPos
{
    int col;
    int row;
};

namespace Furniture
{
    const int FurnitureWidth = 32;
    const int FurnitureHeight = 32;
    const int cols = 800 / FurnitureWidth;
    const int rows = 600 / FurnitureHeight;
    const int offsetX = 0;
    const int offsetY = 10;
}

class FurnitureGrid
{
    public:
    FurnitureGrid():pos{0,0},type{FurnitureType::None},
    placement{PlacementType::Kitchen},id{-1}{};          
    ~FurnitureGrid() = default;

    void InitFurnitureGrid(GridPos pos, FurnitureType type, PlacementType placement, int id)
    {
        this->pos = pos;
        this->type = type;
        this->placement = placement;
        this->id = id;
        Rect.x = pos.row * Furniture::FurnitureWidth + Furniture::offsetX;
        Rect.y = pos.col * Furniture::FurnitureHeight + Furniture::offsetY;
        Rect.w = Furniture::FurnitureWidth;
        Rect.h = Furniture::FurnitureHeight;
        boxTexture = ResourceManager::instance()->FindTexture("PlacementBox");
    }
    int GetX()
    {
        return pos.row * Furniture::FurnitureWidth + Furniture::offsetX;   
    }
    int GetY()
    {
        return pos.col * Furniture::FurnitureHeight + Furniture::offsetY;       
    }
    int GetWidth()
    {
        return Furniture::FurnitureWidth;   
    }
    int GetHeight()
    {
        return Furniture::FurnitureHeight;   
    }

    void SetPos(GridPos pos)
    {
        this->pos = pos;
    }

    void SetPos(int x, int y)
    {
        this->pos.col = y / Furniture::FurnitureHeight;
        this->pos.row = x / Furniture::FurnitureWidth;  
    }

    void SetType(FurnitureType type)
    {
        this->type = type;  
    }

    void SetPlacement(PlacementType placement)
    {
        this->placement = placement;  
    }

    void SetID(int id)
    {
        this->id = id;
    }

    FurnitureType GetType()
    {
        return type;
    }

    PlacementType GetPlacement()
    {
        return placement;
    }

    GridPos GetPos()
    {
        return pos;
    }

    void onRender(SDL_Renderer* renderer)
    {
        if (type == FurnitureType::None)
        {
            // 空格子：绿色网格，ALPHA=50
            SDL_SetTextureColorMod(boxTexture, 0, 255, 0);
            SDL_SetTextureAlphaMod(boxTexture, 255);
            SDL_RenderCopy(renderer, boxTexture, nullptr, &Rect);
        }
        else
        {
            // 已占用：红色网格，不渲染原始材质
            SDL_SetTextureColorMod(boxTexture, 255, 0, 0);
            SDL_SetTextureAlphaMod(boxTexture, 255);
            SDL_RenderCopy(renderer, boxTexture, nullptr, &Rect);
        }
        // 恢复默认值
        SDL_SetTextureColorMod(boxTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(boxTexture, 255);
    }

    private:
    GridPos pos;
    FurnitureType type;
    PlacementType placement;
    int id;
    SDL_Rect Rect;
    SDL_Texture* boxTexture;
};

// ===== 桌椅套装：记录一套桌椅的网格位置 =====
// 每张桌子 / 椅子各占 1 个 32×32 格子，渲染时放大到 64×64
class DeskChairSet
{
public:
    DeskChairSet() = default;
    ~DeskChairSet() = default;

    void InitSet(int id, int deskCnt, int chairCnt, PlacementType place)
    {
        setID = id;
        deskCount = deskCnt;
        chairCount = chairCnt;
        placement = place;
        deskPositions.resize(deskCnt);
        chairPositions.resize(chairCnt);
    }

    int GetSetID()        const { return setID; }
    int GetDeskCount()    const { return deskCount; }
    int GetChairCount()   const { return chairCount; }
    int GetTotalCells()   const { return deskCount + chairCount; }
    PlacementType GetPlacement() const { return placement; }

    void SetDeskPos(int index, GridPos pos)
    {
        if (index >= 0 && index < deskCount)
            deskPositions[index] = pos;
    }

    void SetChairPos(int index, GridPos pos)
    {
        if (index >= 0 && index < chairCount)
            chairPositions[index] = pos;
    }

    GridPos GetDeskPos(int index) const
    {
        if (index >= 0 && index < deskCount)
            return deskPositions[index];
        return {0, 0};
    }

    GridPos GetChairPos(int index) const
    {
        if (index >= 0 && index < chairCount)
            return chairPositions[index];
        return {0, 0};
    }

    // 将格点转为像素坐标（含 Furniture 偏移）
    static int GridToPixelX(int row) { return row * Furniture::FurnitureWidth  + Furniture::offsetX; }
    static int GridToPixelY(int col) { return col * Furniture::FurnitureHeight + Furniture::offsetY; }

    int GetDeskPixelX(int index) const { return GridToPixelX(GetDeskPos(index).row); }
    int GetDeskPixelY(int index) const { return GridToPixelY(GetDeskPos(index).col); }
    int GetChairPixelX(int index) const { return GridToPixelX(GetChairPos(index).row); }
    int GetChairPixelY(int index) const { return GridToPixelY(GetChairPos(index).col); }

private:
    int setID = 0;
    int deskCount = 0;
    int chairCount = 0;
    PlacementType placement = PlacementType::Eating;
    std::vector<GridPos> deskPositions;
    std::vector<GridPos> chairPositions;
};