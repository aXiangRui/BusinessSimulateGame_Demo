#pragma once

#include "RUI_Furniture.h"

// ===== 放置条目 —— PlacementManager 的数据单元 =====
// 描述一个已放置家具的完整信息

struct PlacementEntry
{
    int           id;        // 同类家具内唯一 ID（自增）
    FurnitureType type;      // 家具类型（复用 RUI_Furniture.h 枚举）
    GridPos       pos;       // 网格坐标（复用 RUI_Furniture.h 的 GridPos）
    bool          flipped;   // 是否水平翻转
    PlacementType zone;      // 所属区域（Kitchen / Showing / Eating）

    // 转为像素坐标（格点左上角，与家具存储位置一致）
    int PixelX() const
    {
        return pos.row * Furniture::FurnitureWidth + Furniture::offsetX;
    }
    int PixelY() const
    {
        return pos.col * Furniture::FurnitureHeight + Furniture::offsetY;
    }
};
