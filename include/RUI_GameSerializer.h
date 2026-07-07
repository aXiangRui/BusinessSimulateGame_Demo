#pragma once

#include <vector>
#include <string>
#include "RUI_Customer.h"
#include "RUI_Cabinet.h"
#include "RUI_Clock.h"
#include "RUI_Cook.h"
#include "RUI_StoreServer.h"
#include "RUI_CustomerManager.h"
#include "RUI_Furniture.h"
#include "RUI_Chair.h"

// 游戏存档读写 —— 从 GameWorld 中分离出来的独立模块
class GameSerializer
{
public:
    // 从存档文件加载游戏状态
    static void Load(
        Clock& timeClock,
        std::vector<Customer>& customers,
        std::vector<Cook>& cooks,
        std::vector<StoreServer>& servers,
        std::vector<Cabinet>& cabinets,
        CustomerManager& customerManager,
        int& totalMoney,
        int& totalCustomers,
        int& totalDessert
    );

    // 将游戏状态写入存档文件
    static void Save(
        Clock& timeClock,
        std::vector<Customer>& customers,
        std::vector<Cook>& cooks,
        std::vector<StoreServer>& servers,
        std::vector<Cabinet>& cabinets,
        int totalMoney,
        int totalCustomers,
        int totalDessert
    );

    // 桌椅套装存取
    static void LoadDeskChairSets(std::vector<DeskChairSet>& sets);
    static void SaveDeskChairSets(const std::vector<DeskChairSet>& sets);
};
