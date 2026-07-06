#include "../include/RUI_TimeOfDaySystem.h"
#include <SDL2/SDL.h>

int TimeOfDaySystem::CalculateAlpha(int presentHour, float hourProgress)
{
    // 晚上逐渐变黑（18:00 - 22:00）
    if (presentHour >= 18 && presentHour < 22)
    {
        float totalProgress = (presentHour - 18) + hourProgress;
        float maxProgress = 22 - 18;
        float normalized = totalProgress / maxProgress;
        int alpha = static_cast<int>(normalized * MaxAlpha);
        return (alpha > MaxAlpha) ? MaxAlpha : alpha;
    }
    // 深夜保持黑暗（22:00 - 4:00）
    else if (presentHour >= 22 || presentHour < 4)
    {
        return MaxAlpha;
    }
    // 早晨逐渐变亮（4:00 - 7:00）
    else if (presentHour >= 4 && presentHour < 7)
    {
        float totalProgress = (presentHour - 4) + hourProgress;
        float maxProgress = 7 - 4;
        float normalized = totalProgress / maxProgress;
        int alpha = static_cast<int>((1.0f - normalized) * MaxAlpha);
        return (alpha < 0) ? 0 : alpha;
    }
    // 白天保持明亮（7:00 - 18:00）
    else
    {
        return 0;
    }
}

void TimeOfDaySystem::Update(
    Clock& clock,
    State& state,
    int customerCount,
    int& totalMoney,
    int& totalDessert,
    int& totalCustomers,
    std::vector<Cabinet>& cabinets,
    std::vector<Chair>& chairs,
    bool& isSummaryShowing,
    SummaryFrame& summaryFrame)
{
    int presentHour = clock.ReturnHour();
    Uint32 currentTime = SDL_GetTicks();

    // 计算当前小时进度，用于平滑过渡
    Uint32 msSinceLastHour = currentTime - state.lastTime;
    float hourProgress = static_cast<float>(msSinceLastHour) / HourDurationMs;
    if (hourProgress > 1.0f)
    {
        hourProgress = 1.0f;
    }

    // 更新黑夜遮罩透明度
    state.currentAlpha = CalculateAlpha(presentHour, hourProgress);

    // 判断时段并选择时钟速度
    bool isDaytime = (presentHour >= 7 && presentHour < 22);

    if (isDaytime)
    {
        // 白天：正常速度
        if (currentTime - state.lastTime >= HourDurationMs)
        {
            state.lastTime = currentTime;
            clock.UpdateTime();
        }
    }
    else
    {
        // 夜晚：根据是否有顾客决定速度
        if (customerCount == 0)
        {
            // 没顾客时快速跳过夜晚
            if (currentTime - state.lastTime >= HourDurationMs / 10)
            {
                state.lastTime = currentTime;
                clock.UpdateTime();

                // 午夜结算
                if (clock.ReturnHour() == 0)
                {
                    summaryFrame.update(totalCustomers, totalDessert);
                    isSummaryShowing = true;
                    totalMoney = totalMoney - 10 * totalDessert;
                    SDL_Log("今日卖出甜品%d份", totalDessert);
                    SDL_Log("今日顾客共有%d人", totalCustomers);

                    // 补充面包柜库存（不足20的扣钱补货）
                    for (int i = 0; i < (int)cabinets.size(); i++)
                    {
                        if (cabinets[i].GetDessertNumber() < 20)
                        {
                            totalMoney -= 10 * (20 - cabinets[i].GetDessertNumber());
                            cabinets[i].SetDessertNumber(20);
                        }
                    }

                    // 重置所有椅子
                    for (int i = 0; i < 16; i++)
                    {
                        chairs[i].SetUsing(0);
                    }
                }

                // 早上6点重置每日统计
                if (clock.ReturnHour() == 6)
                {
                    totalDessert = 0;
                    totalCustomers = 0;
                    isSummaryShowing = false;
                }
            }
        }
        else
        {
            // 有顾客时放慢速度
            if (currentTime - state.lastTime >= HourDurationMs * 10)
            {
                state.lastTime = currentTime;
                clock.UpdateTime();
            }
        }
    }
}
