#pragma once

#include <vector>
#include <string>
#include "RUI_PlacementData.h"

// ===== 放置管理器 =====
// 统一管理所有可放置家具的位置数据
// 是位置信息的唯一数据源（single source of truth）

class PlacementManager
{
public:
    PlacementManager() = default;
    ~PlacementManager() = default;

    // ===== 加载 / 保存 =====

    /// 从 save/Furniture.rui 加载；不存在则调用 GenerateDefaults()
    void Load();

    /// 写入 save/Furniture.rui
    void Save() const;

    /// 用现有公式生成默认布局（与当前硬编码位置完全一致）
    void GenerateDefaults();

    // ===== 查询 =====

    /// 按类型筛选条目
    std::vector<PlacementEntry> GetByType(FurnitureType type) const;

    /// 按 id+类型查找
    const PlacementEntry* FindById(int id, FurnitureType type) const;

    /// 获取全部条目（只读）
    const std::vector<PlacementEntry>& GetEntries() const { return m_entries; }

    /// 获取条目数量
    int GetCount() const { return (int)m_entries.size(); }

    // ===== 验证 =====

    /// 该格点是否已被占用（遍历全部条目）
    bool IsOccupied(GridPos pos) const;

    /// 格点是否在屏幕范围内（0 ≤ col < cols, 0 ≤ row < rows）
    static bool IsInBounds(GridPos pos);

    // ===== 修改 =====

    /// 尝试放置；失败返回 false（位置被占 / 越界）
    bool TryPlace(int id, FurnitureType type, GridPos pos, bool flipped, PlacementType zone);

    /// 移除指定条目
    bool TryRemove(int id, FurnitureType type);

    /// 获取某类型的下一个自增 ID
    int GetNextId(FurnitureType type) const;

    /// 清空全部条目
    void Clear() { m_entries.clear(); }

private:
    std::vector<PlacementEntry> m_entries;
};
