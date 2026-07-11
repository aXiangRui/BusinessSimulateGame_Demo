#include "../include/RUI_PlacementManager.h"
#include <fstream>
#include <SDL2/SDL.h>

// ===== 内部辅助 =====

namespace
{
    const int  CELL_SIZE    = Furniture::FurnitureWidth;   // 32
    const int  OFFSET_X     = Furniture::offsetX;          // 0
    const int  OFFSET_Y     = Furniture::offsetY;          // 10
    const int  MAX_CABINETS = 24;
    const int  MAX_DESKS    = 8;
    const int  MAX_CHAIRS   = 16;

    /// 将像素坐标吸附到格点左上角
    int SnapPixel(int value, int offset)
    {
        return ((value - offset + CELL_SIZE / 2) / CELL_SIZE) * CELL_SIZE + offset;
    }

    /// 像素 → 格点
    GridPos PixelToGridPos(int px, int py)
    {
        return {
            (py - OFFSET_Y) / CELL_SIZE,
            (px - OFFSET_X) / CELL_SIZE
        };
    }
}

// ===== 加载 / 保存 =====

void PlacementManager::Load()
{
    m_entries.clear();

    std::ifstream file("./save/Furniture.rui");
    if (!file.is_open())
    {
        SDL_Log("PlacementManager: save/Furniture.rui 不存在，使用默认布局");
        GenerateDefaults();
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // 跳过空行和注释
        if (line.empty() || line[0] == '#')
            continue;

        int typeInt, id, col, row, flippedInt, zoneInt;
        if (sscanf(line.c_str(), "%d %d %d %d %d %d",
                   &typeInt, &id, &col, &row, &flippedInt, &zoneInt) == 6)
        {
            PlacementEntry entry;
            entry.id      = id;
            entry.type    = static_cast<FurnitureType>(typeInt);
            entry.pos     = { col, row };
            entry.flipped = (flippedInt != 0);
            entry.zone    = static_cast<PlacementType>(zoneInt);
            m_entries.push_back(entry);
        }
    }

    SDL_Log("PlacementManager: 从存档加载了 %d 个家具条目", (int)m_entries.size());

    // 如果存档为空，回退到默认
    if (m_entries.empty())
    {
        SDL_Log("PlacementManager: 存档为空，使用默认布局");
        GenerateDefaults();
    }
}

void PlacementManager::Save() const
{
    std::ofstream file("./save/Furniture.rui");
    if (!file)
    {
        SDL_Log("PlacementManager::Save: 无法写入 save/Furniture.rui");
        return;
    }

    file << "# Furniture.rui —— 家具布局存档" << std::endl;
    file << "# 格式: 类型 实体ID 列 行 翻转 区域" << std::endl;
    file << "# 类型: 101=Cabinet 102=Register 201=Table 202=Chair" << std::endl;
    file << "# 翻转: 0=正常 1=水平镜像" << std::endl;
    file << "# 区域: 1=Kitchen 2=Showing 3=Eating" << std::endl;
    file << std::endl;

    for (const auto& e : m_entries)
    {
        file << static_cast<int>(e.type) << " "
             << e.id << " "
             << e.pos.col << " "
             << e.pos.row << " "
             << (e.flipped ? 1 : 0) << " "
             << static_cast<int>(e.zone) << std::endl;
    }

    SDL_Log("PlacementManager: 保存了 %d 个家具条目", (int)m_entries.size());
}

void PlacementManager::GenerateDefaults()
{
    m_entries.clear();

    // ===== 面包柜 (展示区, ≤24个) =====
    for (int i = 0; i < MAX_CABINETS; i++)
    {
        int rawX = (i / 12) * 200 + 100 * (i % 2) + 20;
        int rawY = (i / 2 % 6) * 40 + 200;
        int x = SnapPixel(rawX, OFFSET_X);
        int y = SnapPixel(rawY, OFFSET_Y);

        PlacementEntry entry;
        entry.id      = i;
        entry.type    = FurnitureType::Cabinet;
        entry.pos     = PixelToGridPos(x, y);
        entry.flipped = false;
        entry.zone    = PlacementType::Showing;
        m_entries.push_back(entry);
    }

    // ===== 桌子 (用餐区, 8个) =====
    for (int i = 0; i < MAX_DESKS; i++)
    {
        int rawX = (i / 4) * 200 + 490;
        int rawY = (i % 4) * 100;
        int x = SnapPixel(rawX, OFFSET_X);
        int y = SnapPixel(rawY, OFFSET_Y);

        PlacementEntry entry;
        entry.id      = i;
        entry.type    = FurnitureType::Desk;
        entry.pos     = PixelToGridPos(x, y);
        entry.flipped = false;
        entry.zone    = PlacementType::Eating;
        m_entries.push_back(entry);
    }

    // ===== 椅子 (用餐区, 16个) =====
    for (int i = 0; i < MAX_CHAIRS; i++)
    {
        int rawX = (i / 8) * 200 + 450 + 80 * (i % 2);
        int rawY = (i / 2 % 4) * 100;
        int x = SnapPixel(rawX, OFFSET_X);
        int y = SnapPixel(rawY, OFFSET_Y);

        PlacementEntry entry;
        entry.id      = i;
        entry.type    = FurnitureType::Chair;
        entry.pos     = PixelToGridPos(x, y);
        entry.flipped = (i % 2 == 1);  // 奇数 id 椅子水平翻转
        entry.zone    = PlacementType::Eating;
        m_entries.push_back(entry);
    }

    // ===== 收银台 (展示区, 固定位置) =====
    {
        int x = SnapPixel(350, OFFSET_X);
        int y = SnapPixel(140, OFFSET_Y);

        PlacementEntry entry;
        entry.id      = 0;
        entry.type    = FurnitureType::Register;
        entry.pos     = PixelToGridPos(x, y);
        entry.flipped = false;
        entry.zone    = PlacementType::Showing;
        m_entries.push_back(entry);
    }

    SDL_Log("PlacementManager: 生成默认布局 %d 个条目 "
            "(Cabinet=%d, Table=%d, Chair=%d, Register=%d)",
            (int)m_entries.size(),
            MAX_CABINETS, MAX_DESKS, MAX_CHAIRS, 1);
}

// ===== 查询 =====

std::vector<PlacementEntry> PlacementManager::GetByType(FurnitureType type) const
{
    std::vector<PlacementEntry> result;
    for (const auto& e : m_entries)
    {
        if (e.type == type)
            result.push_back(e);
    }
    return result;
}

const PlacementEntry* PlacementManager::FindById(int id, FurnitureType type) const
{
    for (const auto& e : m_entries)
    {
        if (e.id == id && e.type == type)
            return &e;
    }
    return nullptr;
}

// ===== 验证 =====

bool PlacementManager::IsOccupied(GridPos pos) const
{
    for (const auto& e : m_entries)
    {
        if (e.pos.col == pos.col && e.pos.row == pos.row)
            return true;
    }
    return false;
}

bool PlacementManager::IsInBounds(GridPos pos)
{
    return pos.col >= 0 && pos.col < Furniture::cols
        && pos.row >= 0 && pos.row < Furniture::rows;
}

// ===== 修改 =====

bool PlacementManager::TryPlace(int id, FurnitureType type, GridPos pos,
                                bool flipped, PlacementType zone)
{
    if (!IsInBounds(pos))
    {
        SDL_Log("PlacementManager::TryPlace: 越界 (%d, %d)", pos.col, pos.row);
        return false;
    }
    if (IsOccupied(pos))
    {
        SDL_Log("PlacementManager::TryPlace: 位置 (%d, %d) 已被占用", pos.col, pos.row);
        return false;
    }

    PlacementEntry entry;
    entry.id      = id;
    entry.type    = type;
    entry.pos     = pos;
    entry.flipped = flipped;
    entry.zone    = zone;
    m_entries.push_back(entry);
    return true;
}

bool PlacementManager::TryRemove(int id, FurnitureType type)
{
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
    {
        if (it->id == id && it->type == type)
        {
            m_entries.erase(it);
            return true;
        }
    }
    return false;
}

int PlacementManager::GetNextId(FurnitureType type) const
{
    int maxId = -1;
    for (const auto& e : m_entries)
    {
        if (e.type == type && e.id > maxId)
            maxId = e.id;
    }
    return maxId + 1;
}
