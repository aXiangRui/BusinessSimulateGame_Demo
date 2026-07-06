#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "RUI_Clock.h"
#include "RUI_Chair.h"
#include "RUI_Cabinet.h"
#include "RUI_ChatFrame.h"

// 昼夜循环系统 —— 从 GameScene 中分离出来的时间管理模块
// 负责：黑夜遮罩透明度计算、游戏时间速度控制、每日结算
class TimeOfDaySystem
{
public:
    static const Uint32 HourDurationMs = 10000;   // 白天每游戏小时的毫秒数
    static const int MaxAlpha = 150;              // 黑夜遮罩最大透明度

    struct State
    {
        int currentAlpha = 0;   // 当前黑夜遮罩透明度 (0-150)
        Uint32 lastTime = 0;    // 上次时钟更新的 tick
    };

    // 更新昼夜循环。每帧调用（非聊天/暂停状态）。
    static void Update(
        Clock& clock,
        State& state,
        int customerCount,
        int& totalMoney,
        int& totalDessert,
        int& totalCustomers,
        std::vector<Cabinet>& cabinets,
        std::vector<Chair>& chairs,
        bool& isSummaryShowing,
        SummaryFrame& summaryFrame
    );

private:
    // 根据当前游戏时间计算黑夜遮罩透明度
    static int CalculateAlpha(int presentHour, float hourProgress);
};
