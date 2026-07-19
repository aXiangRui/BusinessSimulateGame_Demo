#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "RUI_ResourceManager.h"
#include <iostream>
#include <string>
#include "RUI_Furniture.h"
#include "RUI_Pathfinding.h"

namespace RUI_Furniture
{
    enum class stage
    {
        choose,
        place
    };
}

struct ChooseFurniture
{
    SDL_Texture* texture;
    SDL_Rect textureRect;
};

class BorderBox
{
    public:
    BorderBox() = default;
    ~BorderBox() = default;
    void InitBorderBox(int mx, int my, int mw, int mh ,std::string add)
    {
        borderBoxTexture = ResourceManager::instance()->FindTexture(add.c_str());
        borderBoxRect = {mx,my,mw,mh};
        isChoosing = false;
        isPlacing = false;
    }

    void SetFurnitureTemplates(FurnitureTemplate* templates, int count)
    {
        furnitureTemplates = templates;
        TemplateCount = count;
        const int iconSize  = 128;
        const int padding   = 32;
        const int startX    = borderBoxRect.x + padding;
        const int startY    = borderBoxRect.y + padding;
        for(int i = 0; i < TemplateCount; i++)
        {
            // 为每个 tile 加载贴图，预览图标取第一个 tile
            for (auto& tile : furnitureTemplates[i].tiles)
                tile.texture = ResourceManager::instance()->FindTexture(tile.address.c_str());
            furnitureTemplates[i].previewTexture = furnitureTemplates[i].tiles.empty()
                ? nullptr : furnitureTemplates[i].tiles[0].texture;

            int col = 0 ;                          // 每行 1 个
            int row = i ;
            SDL_Rect iconRect = {
                startX + col * (iconSize + padding),
                startY + row * (iconSize + padding),
                iconSize, iconSize
            };
            furnitureTemplates[i].previewRect = iconRect;
        }
    }
    // 鼠标移动：检测悬停在哪一个模板图标上
    void onMotionInput(bool isFurniturePlacing, const int mx, const int my)
    {
        if (!isFurniturePlacing && !isChoosing)
        {
            isChoosing = false;
            hoveredIndex = -1;
            return;
        }

        if(isPlacing && chooseFurniture.texture != nullptr)
        {
            chooseFurniture.textureRect.x = mx - chooseFurniture.textureRect.w / 2;
            chooseFurniture.textureRect.y = my - chooseFurniture.textureRect.h / 2;
        }
        

        // 先判断是否在面板区域内
        if (mx >= borderBoxRect.x && mx <= borderBoxRect.x + borderBoxRect.w &&
            my >= borderBoxRect.y && my <= borderBoxRect.y + borderBoxRect.h)
        {
            isChoosing = true;
            hoveredIndex = -1;
            for (int i = 0; i < TemplateCount; i++)
            {
                const SDL_Rect& r = furnitureTemplates[i].previewRect;
                if (mx >= r.x && mx <= r.x + r.w &&
                    my >= r.y && my <= r.y + r.h)
                {
                    hoveredIndex = i;
                    break;
                }
            }
        }
        else
        {
            isChoosing = false;
            hoveredIndex = -1;
        }
    }

    // 点击：选中家具模板，进入放置预览状态
    void onClickInput(bool isFurniturePlacing, const int bx, const int by)
    {
        if (!isFurniturePlacing || !isChoosing) return;

        if(currentStage == RUI_Furniture::stage::choose)
        {
            for (int i = 0; i < TemplateCount; i++)
            {
                const SDL_Rect& r = furnitureTemplates[i].previewRect;
                if (bx >= r.x && bx <= r.x + r.w &&
                    by >= r.y && by <= r.y + r.h)
                {
                    selectedIndex = i;
                    isPlacing = true;   // 进入"放置预览"状态
                    SDL_Log("选中家具: %s", furnitureTemplates[i].name.c_str());
                    chooseFurniture.texture = furnitureTemplates[i].previewTexture;
                    chooseFurniture.textureRect = furnitureTemplates[i].previewRect;
                    chooseFurniture.textureRect.w = 64;
                    chooseFurniture.textureRect.h = 64;
                    currentStage = RUI_Furniture::stage::place;
                    return;
                }
            }
        }
    }

    // 获取当前选中的模板（供 GameScene 读取，用于放置预览和确认放置）
    int GetSelectedIndex() const { return selectedIndex; }
    const FurnitureTemplate* GetSelectedTemplate() const
    {
        if (selectedIndex >= 0 && selectedIndex < TemplateCount)
            return &furnitureTemplates[selectedIndex];
        return nullptr;
    }

    void SetTextureByIndex(int index)
    {
        chooseFurniture.texture = furnitureTemplates[index].previewTexture;
        chooseFurniture.textureRect = furnitureTemplates[index].previewRect;
    }
    bool IsPlacing() const { return isPlacing; }
    bool IsInPanel(int mx, int my) const
    {
        if (isPlacing) return false;  // 放置预览时面板不可见，不拦截
        return mx >= borderBoxRect.x && mx <= borderBoxRect.x + borderBoxRect.w
            && my >= borderBoxRect.y && my <= borderBoxRect.y + borderBoxRect.h;
    }
    void ConfirmPlace() { isPlacing = false; selectedIndex = -1; currentStage = RUI_Furniture::stage::choose; }
    void CancelPlace()  { isPlacing = false; selectedIndex = -1; currentStage = RUI_Furniture::stage::choose; }

    // 设置吸附到网格的预览位置（由 GameScene 每帧调用）
    // 传入锚点格点，内部遍历所有 tile 计算各自的预览位置
    void SetPreviewPos(GridPos anchor, bool valid)
    {
        const FurnitureTemplate* tpl = GetSelectedTemplate();
        previewRects.clear();
        if (!tpl) return;

        for (const auto& tile : tpl->tiles)
        {
            GridPos tilePos;
            tilePos.col = anchor.col + tile.offsetCol;
            tilePos.row = anchor.row + tile.offsetRow;

            int px = GridToPixelX(tilePos.row);
            int py = GridToPixelY(tilePos.col);

            // Chair/Desk 比 Cabinet 多 16px 上移
            if (tile.type == FurnitureType::Chair || tile.type == FurnitureType::Desk)
                py -= 16;

            SDL_Rect r = {px - 16, py - 16, 64, 64};
            previewRects.push_back({tile.texture, r});
        }
        previewValid = valid;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(!isChoosing && !isPlacing)
        {
            return;
        }
        if(!isPlacing)
        {
            SDL_RenderCopy(Renderer,borderBoxTexture,nullptr,&borderBoxRect);

            for (int i = 0; i < TemplateCount; i++)
            {
                SDL_RenderCopy(Renderer,furnitureTemplates[i].previewTexture,nullptr,&furnitureTemplates[i].previewRect);
            }
        }
        else
        {
            // 半透明预览：绿色=可放置，红色=不可放置
            for (const auto& pr : previewRects)
            {
                if (previewValid)
                    SDL_SetTextureColorMod(pr.texture, 100, 255, 100);   // 绿
                else
                    SDL_SetTextureColorMod(pr.texture, 255, 100, 100);   // 红
                SDL_SetTextureAlphaMod(pr.texture, 160);                  // 半透明

                SDL_RenderCopy(Renderer, pr.texture, nullptr, &pr.rect);

                // 恢复默认
                SDL_SetTextureColorMod(pr.texture, 255, 255, 255);
                SDL_SetTextureAlphaMod(pr.texture, 255);
            }
        }

    }
    RUI_Furniture::stage GetCurrentStage() const { return currentStage; }

    int GetCostByIndex(int index) const { return furnitureTemplates[index].cost; }
    int GetCurrentCost() const {return GetCostByIndex(selectedIndex); }

    void onUpdate()
    {

    }

    void onQuit()
    {

    }

    private:
    struct PreviewData
    {
        SDL_Texture* texture;
        SDL_Rect rect;
    };

    SDL_Texture* borderBoxTexture;
    SDL_Rect borderBoxRect;
    bool isChoosing;
    bool isPlacing;
    int hoveredIndex = -1;
    int selectedIndex = -1;
    FurnitureTemplate* furnitureTemplates;
    int TemplateCount = 0;
    ChooseFurniture chooseFurniture;
    RUI_Furniture::stage currentStage = RUI_Furniture::stage::choose;
    std::vector<PreviewData> previewRects;
    bool previewValid = false;
};
