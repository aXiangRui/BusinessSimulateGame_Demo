#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "RUI_ResourceManager.h"
#include <iostream>
#include <string>
#include "RUI_Furniture.h"

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
            furnitureTemplates[i].texture = ResourceManager::instance()->FindTexture(furnitureTemplates[i].address.c_str());
            int col = 0 ;                          // 每行 1 个
            int row = i ;
            SDL_Rect iconRect = {
                startX + col * (iconSize + padding),
                startY + row * (iconSize + padding),
                iconSize, iconSize
            };
            furnitureTemplates[i].textureRect = iconRect;
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
                const SDL_Rect& r = furnitureTemplates[i].textureRect;
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

        for (int i = 0; i < TemplateCount; i++)
        {
            const SDL_Rect& r = furnitureTemplates[i].textureRect;
            if (bx >= r.x && bx <= r.x + r.w &&
                by >= r.y && by <= r.y + r.h)
            {
                selectedIndex = i;
                isPlacing = true;   // 进入"放置预览"状态
                SDL_Log("选中家具: %s", furnitureTemplates[i].name.c_str());
                chooseFurniture.texture = furnitureTemplates[i].texture;
                chooseFurniture.textureRect = furnitureTemplates[i].textureRect;
                chooseFurniture.textureRect.w = 64;
                chooseFurniture.textureRect.h = 64;
                return;
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
        chooseFurniture.texture = furnitureTemplates[index].texture;
        chooseFurniture.textureRect = furnitureTemplates[index].textureRect;
    }
    bool IsPlacing() const { return isPlacing; }
    void ConfirmPlace() { isPlacing = false; selectedIndex = -1; }
    void CancelPlace()  { isPlacing = false; selectedIndex = -1; }

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
                SDL_RenderCopy(Renderer,furnitureTemplates[i].texture,nullptr,&furnitureTemplates[i].textureRect);
            }
        }
        else
        {
            SDL_RenderCopy(Renderer,chooseFurniture.texture,nullptr,&chooseFurniture.textureRect);
        }

    }

    void onUpdate()
    {

    }

    void onQuit()
    {

    }

    private:
    SDL_Texture* borderBoxTexture;
    SDL_Rect borderBoxRect;
    bool isChoosing;
    bool isPlacing;
    int hoveredIndex = -1;      // 当前鼠标悬停的模板索引，-1 表示无
    int selectedIndex = -1;     // 当前选中的模板索引，-1 表示未选中
    FurnitureTemplate* furnitureTemplates;
    int TemplateCount = 0;
    ChooseFurniture chooseFurniture;
};
