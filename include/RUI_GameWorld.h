#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <queue>
#include "RUI_Customer.h"
#include "RUI_Clock.h"
#include "RUI_Cabinet.h"
#include "RUI_CustomerManager.h"
#include "RUI_DessertManager.h"
#include "RUI_ProductManager.h"
#include "RUI_MaterialManager.h"
#include "RUI_ChatFrame.h"
#include "RUI_Cook.h"
#include "RUI_StoreServer.h"
#include "RUI_Chair.h"

// 游戏世界 —— 管理所有游戏实体的状态和更新逻辑
// 从 GameEvent 重构而来，Load/Save 已提取到 GameSerializer
class GameWorld
{
public:
    GameWorld() = default;
    ~GameWorld() = default;

    // ===== 初始化 / 退出 =====
    void OnEnter();
    void Quit();

    // ===== 核心更新 =====
    void OnUpdate(std::vector<Chair>& chairs,
                  std::vector<Cabinet>& cabinets,
                  CustomerManager& customerManager,
                  DessertManager& dessertManager,
                  Register& reg,
                  int& totalMoney,
                  int& totalCustomers,
                  int& totalDessert,
                  int clockTime);

    // ===== 输入 =====
    void HandleInput(const SDL_Event& event);

    // ===== 渲染 =====
    void RenderCustomers(SDL_Renderer* renderer);
    void RenderCooks(SDL_Renderer* renderer);
    void RenderSmallCakes(SDL_Renderer* renderer);
    void RenderServers(SDL_Renderer* renderer);
    void RenderCabinetFrame(SDL_Renderer* renderer, CabinetFrame& cabinetFrame,
                            const std::vector<Cabinet>& cabinets);
    void RenderProductPage(SDL_Renderer* renderer, int page,
                           DessertManager& dessertManager, MaterialManager& materialManager);
    void RenderSettingProduct(SDL_Renderer* renderer, int page,
                              DessertManager& dessertManager, MaterialManager& materialManager);
    void RenderCustomerFrame(SDL_Renderer* renderer);

    // ===== 顾客管理 =====
    void AddCustomer(const Customer& c);
    void DeleteCustomer(int id);
    std::vector<Customer>& GetCustomers()
    {
        return customers;
    }
    void SetCustomers(const std::vector<Customer>& c)
    {
        customers = c;
    }
    int GetCustomerCount()
    {
        return (int)customers.size();
    }

    // ===== 时钟 =====
    void SetClock(const Clock& c)
    {
        clock = c;
    }
    int GetClockTime()
    {
        return clock.ReturnAllHour();
    }

    // ===== 访问器 =====
    std::vector<Cook>& GetCooks()
    {
        return cooks;
    }
    std::vector<StoreServer>& GetServers()
    {
        return servers;
    }
    std::vector<SmallCake>& GetSmallCakes()
    {
        return smallCakes;
    }
    ProductManager& GetProductManager()
    {
        return productManager;
    }

    bool IsReadingPage()
    {
        return isReadingPage != -1;
    }
    int  GetReadingPage()
    {
        return isReadingPage;
    }
    void SetReadingPage(int page)
    {
        isReadingPage = page;
    }

    bool IsRenderingCustomerFrame()
    {
        return showCustomerFrame;
    }
    void SetShowCustomerFrame(bool show)
    {
        showCustomerFrame = show;
    }

    int GetProductCount()
    {
        return productManager.GetProductSize();
    }

private:
    // ===== 内部方法 =====
    void SwapCustomer(Customer& a, Customer& b);
    void RemoveIdFromQueue(int id);
    void SpawnCustomers(CustomerManager& customerManager,
                        int& totalCustomers, int& totalDessert,
                        Uint32 currentTime);
    void UpdateCustomerQueue();
    void ProcessPaymentQueue();
    void UpdateEmployees(Uint32 currentTime,
                         std::vector<Cabinet>& cabinets,
                         Register& reg,
                         int clockTime);

    // ===== 实体容器 =====
    std::vector<Customer> customers;
    std::queue<int> payQueue;
    std::vector<Cook> cooks;
    std::vector<StoreServer> servers;
    std::vector<SmallCake> smallCakes;
    std::vector<Plate> plates;

    // ===== 待加入顾客（带延迟） =====
    std::vector<Customer> pendingCustomers;

    // ===== 管理器 =====
    ProductManager productManager;

    // ===== UI 组件 =====
    ChatFrame chat;
    CustomerFrame customerFrame;

    // ===== 时间 =====
    Clock clock;
    Uint32 currentTime = 0;
    Uint32 lastSpawnTime = 0;

    // ===== 状态标志 =====
    int isReadingPage = -1;
    bool showCustomerFrame = false;
    int frameRenderTime = 0;
    bool stillShow = false;
    int cakeTimer = 0;
};
