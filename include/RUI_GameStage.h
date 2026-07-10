#pragma once

// 基础 UI 状态（互斥）：同一时刻只能处于一个状态
// Chat 和 Summary 是叠加层，不在此枚举中
enum class GameStage
{
    Normal,             // 默认游戏画面
    ReadProduct,        // 浏览产品页
    SettingNewProduct,  // 面包柜详情 + 设置按钮
    CheckingProduct,    // 选择产品网格
};