#include "../include/RUI_GameWorld.h"
#include "../include/RUI_ResourceManager.h"
#include <SDL2/SDL.h>

// ===== 初始化 / 退出 ============================================================

void GameWorld::OnEnter()
{
    productManager.InitProductManager();

    for (int i = 0; i < 3; i++)
    {
        Plate a;
        a.InitPlate(200, 200, i);
        plates.push_back(a);
    }
    chat.init();
    isReadingPage = -1;
    customerFrame.Init();
    cakeTimer = 0;
}

void GameWorld::Quit()
{
    productManager.quit();
    plates.clear();
    cooks.clear();
    servers.clear();
    customers.clear();
    pendingCustomers.clear();
    smallCakes.clear();
}

// ===== 顾客管理 ================================================================

void GameWorld::SwapCustomer(Customer& a, Customer& b)
{
    Customer c = a;
    a = b;
    b = c;
}

void GameWorld::AddCustomer(const Customer& c)
{
    customers.push_back(c);
}

void GameWorld::DeleteCustomer(int id)
{
    for (size_t i = 0; i < customers.size(); i++)
    {
        if (customers[i].GetCustomerID() == id)
        {
            SwapCustomer(customers[i], customers[customers.size() - 1]);
            customers[customers.size() - 1].Clean();
            customers.pop_back();
            SDL_Log("删除顾客: removed id=%d", id);
            RemoveIdFromQueue(id);
            return;
        }
    }
}

void GameWorld::RemoveIdFromQueue(int id)
{
    if (payQueue.empty())
    {
        return;
    }
    std::queue<int> newQ;
    while (!payQueue.empty())
    {
        int v = payQueue.front();
        payQueue.pop();
        if (v != id)
        {
            newQ.push(v);
        }
    }
    payQueue = std::move(newQ);
}

// ===== 输入处理 ================================================================

void GameWorld::HandleInput(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    {
        if (showCustomerFrame)
        {
            showCustomerFrame = false;
            frameRenderTime = 0;
            stillShow = true;
        }
        int mx = event.button.x;
        int my = event.button.y;
        for (int i = 0; i < (int)customers.size(); i++)
        {
            if (mx >= customers[i].getX() && mx <= customers[i].getX() + 64
                && !showCustomerFrame && !stillShow)
            {
                if (my >= customers[i].getY() && my <= customers[i].getY() + 64)
                {
                    std::string dessertAddress;
                    if (customers[i].GetPreferDessertID() != -1)
                    {
                        dessertAddress = "";
                    }
                    else
                    {
                        dessertAddress = "nullptr";
                    }
                    customerFrame.SetCustomer(customers[i].GetCustomerName(), dessertAddress);
                    showCustomerFrame = true;
                    break;
                }
            }
        }
        stillShow = false;
        break;
    }
    case SDL_MOUSEMOTION:
    {
        int mx = event.motion.x;
        int my = event.motion.y;
        for (int i = 0; i < (int)customers.size(); i++)
        {
            bool isHover = false;
            int cx = customers[i].getX();
            int cy = customers[i].getY();
            if (mx >= cx && mx <= cx + 64 && my >= cy && my <= cy + 64)
            {
                isHover = true;
            }
            customers[i].SetIsHovered(isHover);
        }
        break;
    }
    }
}

// ===== 核心更新 ================================================================

void GameWorld::SpawnCustomers(CustomerManager& customerManager,
                                int& totalCustomers, int& totalDessert,
                                Uint32 currentTime)
{
    // 只在营业时间（7:00-21:00）生成顾客
    if (clock.ReturnHour() < 7 || clock.ReturnHour() >= 21)
    {
        return;
    }

    if (currentTime - lastSpawnTime < 7000 + (rand() % 2000) - 1000)
    {
        return;
    }

    for (int i = 0; i < customerManager.GetCustomersSize(); i++)
    {
        if (customerManager.Customers[i].GetWhetherAppear())
        {
            if (customerManager.Customers[i].WhetherAdd((int)customers.size(), clock))
            {
                SDL_Log("%s的值为%d",
                    customerManager.Customers[i].GetCustomerName().c_str(),
                    customerManager.Customers[i].GetHasJoined());

                Customer a;
                a.InitCustomer(
                    totalCustomers,
                    customerManager.GetPreferDessertID(i),
                    customerManager.GetCustomerName(i),
                    customerManager.GetCustomerPath(i),
                    customerManager.GetWhetherAppear(i),
                    customerManager.GetCustomerPreference(i));
                a.SetChooseNumber();
                pendingCustomers.push_back(a);

                customerManager.Customers[i].SetHasJoined(1);
                totalCustomers++;
                totalDessert += a.GetChooseNumber();
                lastSpawnTime = currentTime;
            }
        }
    }
}

void GameWorld::UpdateCustomerQueue()
{
    // 检测需要排队的顾客
    for (int i = 0; i < (int)customers.size(); i++)
    {
        if (customers[i].isGoingToPay(currentTime))
        {
            customers[i].setIsGoingToPay(1);
            payQueue.push(customers[i].GetCustomerID());
        }
    }

    // 更新队列位置
    std::queue<int> tmp = payQueue;
    int pos = 0;
    while (!tmp.empty())
    {
        int id = tmp.front();
        tmp.pop();
        for (int k = 0; k < (int)customers.size(); ++k)
        {
            if (customers[k].GetCustomerID() == id)
            {
                customers[k].SetQueueNumber(pos);
                break;
            }
        }
        pos++;
    }
}

void GameWorld::ProcessPaymentQueue()
{
    // 清理队列中不存在的顾客
    while (!payQueue.empty())
    {
        int peekId = payQueue.front();
        bool exists = false;
        for (int k = 0; k < (int)customers.size(); ++k)
        {
            if (customers[k].GetCustomerID() == peekId)
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            payQueue.pop();
        }
        else
        {
            break;
        }
    }

    if (payQueue.empty())
    {
        return;
    }

    int frontId = payQueue.front();
    int idx = -1;
    for (int k = 0; k < (int)customers.size(); ++k)
    {
        if (customers[k].GetCustomerID() == frontId)
        {
            idx = k;
            break;
        }
    }

    if (idx == -1)
    {
        payQueue.pop();
        return;
    }

    Customer& front = customers[idx];
    // 清除其他人的队首标志
    for (int k = 0; k < (int)customers.size(); ++k)
    {
        if (k != idx)
        {
            customers[k].setIsfront(false);
        }
    }
    front.setIsfront(true);

    if (front.GetPayTime() == 0)
    {
        front.SetPayTime(currentTime);
    }

    if (front.hasPayed())
    {
        front.setIsfront(false);
        payQueue.pop();
    }
}

void GameWorld::UpdateEmployees(Uint32 currentTime,
                                 std::vector<Cabinet>& cabinets,
                                 Register& reg,
                                 int clockTime)
{
    // 更新厨师
    for (int i = 0; i < (int)cooks.size(); i++)
    {
        cooks[i].onUpdate(currentTime, smallCakes, cabinets, clockTime);
    }

    // 更新服务员
    for (int i = 0; i < (int)servers.size(); i++)
    {
        servers[i].update(currentTime, smallCakes, cabinets, reg,
                          (int)payQueue.size());
    }

    // 确保只有一个服务员在收银
    int payServer = 0;
    for (int i = 0; i < (int)servers.size(); i++)
    {
        if (payServer > 0 && servers[i].GetCurrentStage() == 1)
        {
            servers[i].SetCurrentStage(0);
        }
        if (servers[i].GetCurrentStage() == 1 && payServer == 0)
        {
            payServer = 1;
        }
    }
}

void GameWorld::OnUpdate(std::vector<Chair>& chairs,
                          std::vector<Cabinet>& cabinets,
                          CustomerManager& customerManager,
                          DessertManager& dessertManager,
                          Register& reg,
                          int& totalMoney,
                          int& totalCustomers,
                          int& totalDessert,
                          int clockTime,
                          const std::vector<FurnitureGrid>& furnitureGrids)
{
    // 防止溢出
    if (totalCustomers >= 10000000)
    {
        totalCustomers = 0;
    }

    currentTime = SDL_GetTicks();

    // 处理待加入顾客（带延迟的入场效果）
    for (int i = (int)pendingCustomers.size() - 1; i >= 0; --i)
    {
        if (currentTime - lastSpawnTime >= (Uint32)pendingCustomers[i].GetDelayTime())
        {
            customers.push_back(pendingCustomers[i]);
            SwapCustomer(pendingCustomers[i], pendingCustomers[pendingCustomers.size() - 1]);
            pendingCustomers.pop_back();
        }
    }

    // 生成新顾客（加入待处理队列）
    SpawnCustomers(customerManager, totalCustomers, totalDessert, currentTime);

    // 更新每个顾客
    for (int i = (int)customers.size() - 1; i >= 0; --i)
    {
        for (int j = 0; j < customerManager.GetCustomersSize(); j++)
        {
            if (customers[i].GetCustomerName() == customerManager.GetCustomerName(j))
            {
                customers[i].Update(chairs, currentTime, cabinets,
                    dessertManager, productManager,
                    customerManager.Customers[j], totalMoney, reg,
                    furnitureGrids);
            }
        }

        // 离开的顾客
        if (customers[i].GetQuit()
            && customers[i].getX() > 800
            && customers[i].getY() > 350)
        {
            SDL_Log("顾客准备离开，此时id:%d, x:%d, y:%d",
                customers[i].GetCustomerID(),
                customers[i].getX(),
                customers[i].getY());

            std::string name = customers[i].GetCustomerName();
            for (int j = 0; j < customerManager.GetCustomersSize(); j++)
            {
                if (name == customerManager.Customers[j].GetCustomerName())
                {
                    customerManager.Customers[j].SetHasJoined(0);
                }
            }
            DeleteCustomer(customers[i].GetCustomerID());
            SDL_Log("顾客离开，剩下%d人", (int)customers.size());
        }
    }

    // 排队队列管理
    UpdateCustomerQueue();
    ProcessPaymentQueue();

    // 更新员工
    UpdateEmployees(currentTime, cabinets, reg, clockTime);

    // 顾客信息框自动关闭（5秒后）
    if (showCustomerFrame && frameRenderTime == 0)
    {
        frameRenderTime = currentTime;
    }
    if (currentTime - frameRenderTime > 5000 && frameRenderTime != 0)
    {
        showCustomerFrame = false;
        frameRenderTime = 0;
    }
}

// ===== 渲染 ===================================================================

void GameWorld::RenderCustomers(SDL_Renderer* renderer)
{
    for (int i = 0; i < (int)customers.size(); i++)
    {
        if (customers[i].GetIsHoverd())
        {
            customers[i].onRenderWithName(renderer);
        }
        else
        {
            customers[i].OnRender(renderer);
        }

        int stage = customers[i].GetCurrentStage();
        if (stage >= 2 && stage <= 3)
        {
            customers[i].RenderCake(renderer);
        }
    }
}

void GameWorld::RenderCustomer(SDL_Renderer* renderer, int i)
{
    if(i < 0 || i >= (int)customers.size())
    {
        return;
    }
    if (customers[i].GetIsHoverd())
        {
            customers[i].onRenderWithName(renderer);
        }
        else
        {
            customers[i].OnRender(renderer);
        }

        int stage = customers[i].GetCurrentStage();
        if (stage >= 2 && stage <= 3)
        {
            customers[i].RenderCake(renderer);
        }
}

void GameWorld::RenderCooks(SDL_Renderer* renderer)
{
    for (int i = 0; i < (int)cooks.size(); i++)
    {
        cooks[i].onRender(renderer);
    }
}

void GameWorld::RenderSmallCakes(SDL_Renderer* renderer)
{
    for (int i = 0; i < (int)smallCakes.size(); i++)
    {
        smallCakes[i].onRender(renderer);
    }
}

void GameWorld::RenderServers(SDL_Renderer* renderer)
{
    for (int i = 0; i < (int)servers.size(); i++)
    {
        servers[i].onRender(renderer);
    }
}

void GameWorld::RenderCabinetFrame(SDL_Renderer* renderer,
                                    CabinetFrame& cabinetFrame,
                                    const std::vector<Cabinet>& cabinets)
{
    int cid = cabinetFrame.GetCabinetID();
    if (cid >= 0 && cid < (int)cabinets.size())
    {
        cabinetFrame.onRender(renderer, const_cast<Cabinet&>(cabinets[cid]), productManager);
    }
}

void GameWorld::RenderProductPage(SDL_Renderer* renderer, int page,
                                   DessertManager& dessertManager,
                                   MaterialManager& materialManager)
{
    std::string title = "查看产品中,当前第" + std::to_string(page + 1) + "页";
    chat.setTitle(title);
    chat.RenderFrame(renderer);

    for (int i = 0; i < productManager.GetProductSize(); i++)
    {
        if (i / 6 == page)
        {
            productManager.products[i].onRender(
                renderer, dessertManager, materialManager, plates,
                200 + i / 3 * 200 - page * 400,
                40 + i % 3 * 180, 180, 180);
        }
    }
    chat.RenderTitle(renderer);
}

void GameWorld::RenderSettingProduct(SDL_Renderer* renderer, int page,
                                      DessertManager& dessertManager,
                                      MaterialManager& materialManager)
{
    std::string title = "设置产品中,当前第" + std::to_string(page + 1) + "页";
    chat.setTitle(title);
    chat.RenderFrame(renderer);

    for (int i = 0; i < productManager.GetProductSize(); i++)
    {
        if (i / 6 == page)
        {
            productManager.products[i].onRender(
                renderer, dessertManager, materialManager, plates,
                200 + i / 3 * 200 - page * 400,
                40 + i % 3 * 180, 180, 180);
        }
    }
    chat.RenderTitle(renderer);
}

void GameWorld::RenderCustomerFrame(SDL_Renderer* renderer)
{
    customerFrame.onRender(renderer);
}
