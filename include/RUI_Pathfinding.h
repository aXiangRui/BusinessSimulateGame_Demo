#pragma once

#include <queue>
#include <vector>
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include "RUI_Furniture.h"

// ===== A* 寻路模块 =====
// 纯函数，零耦合游戏逻辑，只依赖 RUI_Furniture.h 的 GridPos 和 FurnitureGrid

struct PathNode
{
    GridPos pos;
    int gCost;  // 从起点到当前的代价
    int hCost;  // 启发式：到终点的曼哈顿距离估计

    int fCost() const { return gCost + hCost; }

    bool operator>(const PathNode& other) const
    {
        return fCost() > other.fCost();
    }
};

// GridPos 的哈希和相等比较（用于 unordered_map）
struct GridPosHash
{
    size_t operator()(const GridPos& p) const
    {
        return static_cast<size_t>(p.row) * 1000 + static_cast<size_t>(p.col);
    }
};

struct GridPosEqual
{
    bool operator()(const GridPos& a, const GridPos& b) const
    {
        return a.col == b.col && a.row == b.row;
    }
};

// 检查格点是否在网格内且可通行（仅判类型，不判区域）
inline bool IsPassable(GridPos pos, const std::vector<FurnitureGrid>& grid)
{
    for (const auto& cell : grid)
    {
        GridPos cellPos = cell.GetPos();
        if (cellPos.col == pos.col && cellPos.row == pos.row)
        {
            return cell.GetType() == FurnitureType::None;
        }
    }
    return false;  // 不在网格内 → 不可通行
}

// 顾客可通行 = 类型为空 + 不在厨房区域
inline bool IsPassableForCustomer(GridPos pos, const std::vector<FurnitureGrid>& grid)
{
    for (const auto& cell : grid)
    {
        GridPos cellPos = cell.GetPos();
        if( cellPos.col == pos.col && cellPos.row == pos.row && cell.GetPlacement() == PlacementType::Kitchen)
        {
            return false;
        }
        if (cellPos.col == pos.col && cellPos.row == pos.row && cell.GetType() == FurnitureType::None)
        {
            return true;
        }
    }
    return false;
}

// 检查格点是否存在于网格中（不管类型）
inline bool IsInGrid(GridPos pos, const std::vector<FurnitureGrid>& grid)
{
    for (const auto& cell : grid)
    {
        GridPos cellPos = cell.GetPos();
        if (cellPos.col == pos.col && cellPos.row == pos.row)
        {
            return true;
        }
    }
    return false;
}

// 获取格点的家具类型（不在网格中返回 None）
inline FurnitureType GetGridType(GridPos pos, const std::vector<FurnitureGrid>& grid)
{
    for (const auto& cell : grid)
    {
        GridPos cellPos = cell.GetPos();
        if (cellPos.col == pos.col && cellPos.row == pos.row)
        {
            return cell.GetType();
        }
    }
    return FurnitureType::None;
}

// 获取可通行的四方向邻居
// 只有目标椅子允许被当作"可通行"，其他设施对顾客一律不可通行
inline std::vector<GridPos> GetNeighbors(GridPos pos,
                                         const std::vector<FurnitureGrid>& grid,
                                         GridPos end)
{
    std::vector<GridPos> neighbors;
    // 方向顺序：右、上、左、下 —— 优先探索右上方向（收银台在右上角）
    const int dr[] = { 0,  -1,  0, 1};  // row 变化：不动、+1(右)、不动、-1(左)
    const int dc[] = {-1,  0,  1,  0};  // col 变化：-1(上)、不动、+1(下)、不动

    for (int i = 0; i < 4; i++)
    {
        GridPos next = { pos.col + dc[i], pos.row + dr[i] };

        // 只有目标椅子允许临时通行（椅子格子本身被占用）
        if (next.col == end.col && next.row == end.row
            && IsInGrid(next, grid)
            && GetGridType(end, grid) == FurnitureType::Chair)
        {
            neighbors.push_back(next);
        }
        else if (IsPassableForCustomer(next, grid))
        {
            neighbors.push_back(next);
        }
    }
    return neighbors;
}

// A* 寻路：返回从 start 到 end 的格点路径（含首尾）
// 如果无路径，返回空 vector
inline std::vector<GridPos> FindPath(GridPos start, GridPos end,
                                     const std::vector<FurnitureGrid>& grid)
{
    using PQ = std::priority_queue<PathNode, std::vector<PathNode>,
                                   std::greater<PathNode>>;

    PQ openSet;
    std::unordered_map<GridPos, GridPos, GridPosHash, GridPosEqual> cameFrom;
    std::unordered_map<GridPos, int,     GridPosHash, GridPosEqual> gCost;

    openSet.push({ start, 0, 0 });
    gCost[start] = 0;

    while (!openSet.empty())
    {
        GridPos current = openSet.top().pos;
        openSet.pop();

        // 到达终点
        if (current.col == end.col && current.row == end.row)
        {
            std::vector<GridPos> path;
            GridPos p = end;
            while (!(p.col == start.col && p.row == start.row))
            {
                path.push_back(p);
                auto it = cameFrom.find(p);
                if (it == cameFrom.end()) break;  // 安全保护
                p = it->second;
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& next : GetNeighbors(current, grid, end))
        {
            int newG = gCost[current] + 1;

            auto it = gCost.find(next);
            if (it == gCost.end() || newG < it->second)
            {
                gCost[next] = newG;
                int dx = std::abs(next.col - end.col);
                int dy = std::abs(next.row - end.row);
                // 曼哈顿距离 × 1.001 打破平局，优先探索启发值更小的节点
                // 配合邻居顺序（右/上优先），让顾客自然走向右上角的收银台
                int h = (dx + dy) * 1001 / 1000;
                openSet.push({ next, newG, h });
                cameFrom[next] = current;
            }
        }
    }

    return {};  // 无路径
}

// ===== 坐标转换工具 =====
// 格点 32×32，实体（顾客/家具）渲染尺寸 64×64
// 转换时以实体中心为参考，确保格点 ↔ 像素往返一致

const int ENTITY_RENDER_SIZE = 64;
const int GRID_CELL_SIZE     = 32;
const int HALF_ENTITY        = ENTITY_RENDER_SIZE / 2;            // 32
const int CENTER_OFFSET      = (ENTITY_RENDER_SIZE - GRID_CELL_SIZE) / 2;  // 16

// 格点左上角 → 像素（原始，用于与家具格点位置交互）
inline int GridToPixelX(int row)
{
    return row * Furniture::FurnitureWidth + Furniture::offsetX;
}

inline int GridToPixelY(int col)
{
    return col * Furniture::FurnitureHeight + Furniture::offsetY;
}

// 顾客/实体像素（左上角）→ 格点（使用实体中心点）
inline GridPos PixelToGrid(int px, int py)
{
    return {
        (py + HALF_ENTITY - Furniture::offsetY) / Furniture::FurnitureHeight,  // col
        (px + HALF_ENTITY - Furniture::offsetX) / Furniture::FurnitureWidth     // row
    };
}

// 格点 → 实体目标像素（左上角），使 64×64 实体居中于 32×32 格点
inline int GridToEntityPixelX(int row)
{
    return GridToPixelX(row) - CENTER_OFFSET;
}

inline int GridToEntityPixelY(int col)
{
    return GridToPixelY(col) - CENTER_OFFSET;
}

// 家具的格点位置 → 格点（家具 pixel 已吸附到格点左上角，不走中心）
inline GridPos FurniturePixelToGrid(int px, int py)
{
    return {
        (py - Furniture::offsetY) / Furniture::FurnitureHeight,
        (px - Furniture::offsetX) / Furniture::FurnitureWidth
    };
}
