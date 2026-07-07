#include "../include/RUI_GameScene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern int WindowWidth;
extern int WindowHeight;

// ===== 场景生命周期 ============================================================

void RUI_GameScene::onEnter()
{
    // 初始化管理器
    customerManager.InitCustomerManager();
    materialManager.InitMaterialManager();

    // 加载存档
    GameSerializer::Load(clock, world.GetCustomers(),
        world.GetCooks(), world.GetServers(),
        cabinets, customerManager,
        totalMoney, totalCustomers, totalDessert);

    // 初始化游戏世界
    world.OnEnter();
    world.SetReadingPage(0);

    // 初始化场景 UI
    MenuButton btn0((WindowWidth - 320) / 2, 450, 320, 64, "设置新甜点", 0);
    buttons.push_back(btn0);

    BackgroundMusic.quit();
    reg.InitRegister();
    unlockFrame.init();
    icons.Init();

    // 随机背景音乐
    if (!Mix_PlayingMusic())
    {
        if (rand() % 2 == 1)
        {
            gameMusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic"));
            gameMusic.play(-1);
        }
        else
        {
            gameMusic.setMusic(ResourceManager::instance()->FindMusic("gamemusic02"));
            gameMusic.play(-1);
        }
    }

    dessertManager.InitDessertManager();
    textManager.Init();

    // 加载贴图
    background = ResourceManager::instance()->FindTexture("hall");
    backgroundWall = ResourceManager::instance()->FindTexture("hallwall");
    nightTexture = ResourceManager::instance()->FindTexture("night");
    textFont = nullptr;

    cabinetFrame.InitFrame();
    summaryFrame.Init();
    checkEvent.init();

    // ----- 加载 / 创建桌椅套装 -----
    deskChairSets.clear();
    GameSerializer::LoadDeskChairSets(deskChairSets);

    if (deskChairSets.empty())
    {
        // 不存在存档 → 从公式计算默认 8 套（1桌+2椅），吸附到格点
        for (int i = 0; i < 8; i++)
        {
            DeskChairSet set;
            set.InitSet(i, 1, 2, PlacementType::Eating);

            // 桌子位置（复用原公式，吸附到格点）
            Desk tempDesk;
            tempDesk.initDesk(i);
            int dSnapX = ((tempDesk.GetX() - Furniture::offsetX + 16) / 32) * 32 + Furniture::offsetX;
            int dSnapY = ((tempDesk.GetY() - Furniture::offsetY + 16) / 32) * 32 + Furniture::offsetY;
            set.SetDeskPos(0, {dSnapY / 32, dSnapX / 32});  // col = y/32, row = x/32

            // 两把椅子位置
            for (int c = 0; c < 2; c++)
            {
                int cID = i * 2 + c;
                Chair tempChair;
                tempChair.InitChair(cID);
                int cSnapX = ((tempChair.GetX() - Furniture::offsetX + 16) / 32) * 32 + Furniture::offsetX;
                int cSnapY = ((tempChair.GetY() - Furniture::offsetY + 16) / 32) * 32 + Furniture::offsetY;
                set.SetChairPos(c, {cSnapY / 32, cSnapX / 32});
            }

            deskChairSets.push_back(set);
        }
    }

    // 从套装数据创建 Desk / Chair 实体
    chairs.clear();
    desks.clear();
    int chairID = 0;
    int deskID = 0;
    for (int s = 0; s < (int)deskChairSets.size(); s++)
    {
        const DeskChairSet& set = deskChairSets[s];

        for (int d = 0; d < set.GetDeskCount(); d++)
        {
            Desk desk;
            desk.initDesk(deskID++);
            desk.SetPosition(set.GetDeskPixelX(d), set.GetDeskPixelY(d));
            desks.push_back(desk);
        }

        for (int c = 0; c < set.GetChairCount(); c++)
        {
            Chair chair;
            chair.InitChair(chairID++);
            chair.SetPosition(set.GetChairPixelX(c), set.GetChairPixelY(c));
            chairs.push_back(chair);
        }
    }

    // ----- 初始化网格 -----
    furnitureGrids.clear();
    int id = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 25; j++)
        {
            if (i < 13 || j < 13)
            {
                FurnitureGrid grid;
                grid.InitFurnitureGrid({i, j}, FurnitureType::None, PlacementType::Kitchen, id++);
                furnitureGrids.push_back(grid);
            }
        }
    }
    furnitureGrids[136].SetType(FurnitureType::Register);
    furnitureGrids[137].SetType(FurnitureType::Register);

    // 将 Cabinet 与 Grid 绑定
    for (int i = 0; i < (int)cabinets.size(); i++)
    {
        int cabX = cabinets[i].GetX();
        int cabY = cabinets[i].GetY();
        for (int g = 0; g < (int)furnitureGrids.size(); g++)
        {
            if (furnitureGrids[g].GetX() == cabX &&
                furnitureGrids[g].GetY() == cabY)
            {
                furnitureGrids[g].SetType(FurnitureType::Cabinet);
                furnitureGrids[g].SetID(i);
                break;
            }
        }
    }

    // 将 Desk / Chair 与 Grid 绑定
    for (int i = 0; i < (int)desks.size(); i++)
    {
        for (int g = 0; g < (int)furnitureGrids.size(); g++)
        {
            if (furnitureGrids[g].GetX() == desks[i].GetX() &&
                furnitureGrids[g].GetY() == desks[i].GetY())
            {
                furnitureGrids[g].SetType(FurnitureType::Table);
                furnitureGrids[g].SetID(i);
                break;
            }
        }
    }
    for (int i = 0; i < (int)chairs.size(); i++)
    {
        for (int g = 0; g < (int)furnitureGrids.size(); g++)
        {
            if (furnitureGrids[g].GetX() == chairs[i].GetX() &&
                furnitureGrids[g].GetY() == chairs[i].GetY())
            {
                furnitureGrids[g].SetType(FurnitureType::Chair);
                furnitureGrids[g].SetID(i);
                break;
            }
        }
    }

    // 初始化时间和时钟
    timeState = {};
    timeState.lastTime = SDL_GetTicks();
    clock.SetStartTime(world.GetClockTime());
    materialFrame.Init();

    // 重置 UI 状态
    isReadingProduct = false;
    isSettingNewProduct = false;
    isCheckingSetting = false;
    readingPage = -1;
    currentCabinet = -1;
    chatDelayTime = 0;
    isMaterialFrameShowing = false;
    isChatShowing = false;
    isSummaryShowing = false;

    SDL_Log("进入游戏场景");
}

void RUI_GameScene::onUpdate()
{
    Uint32 currentTime = SDL_GetTicks();

    if (!isChatShowing)
    {
        // 更新昼夜循环
        TimeOfDaySystem::Update(clock, timeState,
            world.GetCustomerCount(),
            totalMoney, totalDessert, totalCustomers,
            cabinets, chairs, isSummaryShowing, summaryFrame);

        // 同步时钟到游戏世界并更新
        world.SetClock(clock);
        world.OnUpdate(chairs, cabinets, customerManager,
            dessertManager, reg,
            totalMoney, totalCustomers, totalDessert,
            clock.ReturnHour());
    }
    else
    {
        // 聊天框自动关闭（5秒后）
        if (chatDelayTime == 0)
        {
            chatDelayTime = currentTime;
        }
        if (currentTime - chatDelayTime >= 5000)
        {
            isChatShowing = false;
            chatDelayTime = 0;
        }
    }

    // 解锁检查
    std::vector<Customer> customers = world.GetCustomers();
    checkEvent.update(customerManager, dessertManager,
        materialManager, chatFrame, textManager,
        unlockFrame, isChatShowing, customers);
    world.SetCustomers(customers);
    unlockFrame.SetTime(currentTime);
}

void RUI_GameScene::onRender(SDL_Renderer* renderer)
{
    // 加载字体
    if (!textFont)
    {
        textFont = TTF_OpenFont("./resources/font/namidiansong.ttf", 36);
    }

    // 金额颜色（负数变红）
    SDL_Color moneyColor = textColor;
    if (totalMoney < 0)
    {
        moneyColor = {200, 40, 40, 255};
    }

    // 渲染金额文字
    std::string title = "总金额" + std::to_string(totalMoney);
    SDL_Surface* image = TTF_RenderUTF8_Blended(textFont, title.c_str(), moneyColor);
    SDL_Rect textRect = {10, 60, image->w, image->h};
    SDL_Texture* moneyTexture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, nullptr, &backgroundRect);
    SDL_RenderCopy(renderer, moneyTexture, nullptr, &textRect);
    SDL_DestroyTexture(moneyTexture);

    // 渲染桌椅
    for (int i = 0; i < (int)chairs.size(); i++)
    {
        chairs[i].onRender(renderer);
    }
    for (int i = 0; i < (int)desks.size(); i++)
    {
        desks[i].onRender(renderer);
    }

    // 渲染游戏世界实体
    world.RenderCooks(renderer);
    world.RenderSmallCakes(renderer);
    world.RenderServers(renderer);

    reg.onRender(renderer);
    world.RenderCustomers(renderer);

    // 黑夜遮罩
    SDL_SetTextureAlphaMod(nightTexture, timeState.currentAlpha);
    SDL_RenderCopy(renderer, nightTexture, nullptr, &backgroundRect);

    // 时间和面包柜
    clock.RenderHour(renderer);
    for (int i = 0; i < (int)cabinets.size(); i++)
    {
        cabinets[i].onRender(renderer);
    }

    // 渲染放置框
    if(isFurniturePlacing)
    for( int i = 0; i < furnitureGrids.size(); i++)
    {
        furnitureGrids[i].onRender(renderer);
    }

    // 图标
    icons.onRender(renderer, world.IsReadingPage());

    // 面包柜详情框
    if (cabinetFrame.GetCabinetID() != -1)
    {
        world.RenderCabinetFrame(renderer, cabinetFrame, cabinets);
    }

    // 产品查看页
    if (isReadingProduct)
    {
        world.RenderProductPage(renderer, readingPage, dessertManager, materialManager);
    }

    // 按钮
    for (int i = 0; i < (int)buttons.size(); i++)
    {
        if (isSettingNewProduct && !isReadingProduct)
        {
            buttons[i].ButtonRender(renderer);
        }
    }

    // 材料框
    if (isMaterialFrameShowing)
    {
        materialFrame.onRender(renderer);
    }

    // 产品设置页
    if (isCheckingSetting)
    {
        world.RenderSettingProduct(renderer, readingPage, dessertManager, materialManager);
        icons.Icons[4].onRender(renderer);
        icons.Icons[5].onRender(renderer, 1);
    }

    // 总结框
    if (isSummaryShowing)
    {
        summaryFrame.onRender(renderer);
    }

    unlockFrame.onRender(renderer);

    // 聊天框
    if (isChatShowing)
    {
        chatFrame.RenderFrame(renderer);
        chatFrame.RenderTitle(renderer);
        chatFrame.RenderContent(renderer);
    }

    // 顾客信息框
    if (world.IsRenderingCustomerFrame())
    {
        world.RenderCustomerFrame(renderer);
    }

    SDL_RenderPresent(renderer);
}

void RUI_GameScene::onExit()
{
    SDL_Log("退出游戏场景");

    customerManager.Save();
    materialManager.Save();
    materialManager.quit();

    GameSerializer::Save(clock, world.GetCustomers(),
        world.GetCooks(), world.GetServers(),
        cabinets, totalMoney, totalCustomers, totalDessert);

    GameSerializer::SaveDeskChairSets(deskChairSets);

    buttons.clear();
    chairs.clear();
    desks.clear();
    cabinets.clear();
    deskChairSets.clear();
    furnitureGrids.clear();
    icons.Quit();
    world.Quit();
    dessertManager.Save();
    dessertManager.quit();
    gameMusic.quit();
}

// ===== 输入处理 ================================================================

void RUI_GameScene::onInput(const SDL_Event& event, SDL_Renderer* renderer, bool& running)
{
    world.HandleInput(event);

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    {
        int mx = event.button.x;
        int my = event.button.y;

        // 产品设置点击
        if (isCheckingSetting)
        {
            if (mx >= 200 && mx <= 600 && my >= 0 && my <= 600)
            {
                int offset = ((mx > 400) ? 3 : 0) + (my / 200);
                cabinets[currentCabinet].SetDessertID(readingPage * 6 + offset);
                isCheckingSetting = false;
                isSettingNewProduct = true;
            }
        }

        // 聊天框点击关闭
        if (isChatShowing)
        {
            if (mx >= 200 && mx <= 600)
            {
                isChatShowing = false;
                chatDelayTime = 0;
            }
        }

        // 总结框点击关闭
        if (isSummaryShowing)
        {
            if (mx >= 200 && mx <= 600)
            {
                isSummaryShowing = false;
            }
        }

        // 按钮点击
        for (int i = 0; i < (int)buttons.size(); i++)
        {
            if (isSettingNewProduct)
            {
                if (buttons[i].RUI_isClicked(mx, my))
                {
                    buttons[i].setClicked(true);
                    if (i == 0)
                    {
                        isCheckingSetting = true;
                        readingPage = 0;
                        isSettingNewProduct = false;
                    }
                }
                else
                {
                    buttons[i].setClicked(false);
                }
            }
        }

        // 图标点击
        for (int i = 0; i < (int)icons.Icons.size(); i++)
        {
            if (icons.Icons[i].isClicked(mx, my))
            {
                switch (i)
                {
                case 1: // 烹饪场景
                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Create);
                    break;
                case 2: // 退出到菜单
                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
                    break;
                case 3: // 查看产品
                    if (!isReadingProduct)
                    {
                        isReadingProduct = true;
                        world.SetReadingPage(1);
                        readingPage = 0;
                        if (isMaterialFrameShowing)
                        {
                            isMaterialFrameShowing = false;
                        }
                    }
                    else
                    {
                        isReadingProduct = false;
                        world.SetReadingPage(0);
                        readingPage = -1;
                    }
                    break;
                case 4: // 下一页
                    if (world.IsReadingPage() || isCheckingSetting)
                    {
                        readingPage++;
                        int maxPage = world.GetProductCount() / 6;
                        if (readingPage > maxPage)
                        {
                            readingPage = maxPage;
                        }
                    }
                    break;
                case 5: // 上一页
                    if (world.IsReadingPage() || isCheckingSetting)
                    {
                        readingPage--;
                        if (readingPage < 0)
                        {
                            readingPage = 0;
                        }
                    }
                    break;
                case 6: // 添加面包柜
                    if ((int)cabinets.size() < 24)
                    {
                        Cabinet a;
                        a.InitCabinet((int)cabinets.size(), 0, 0);
                        cabinets.push_back(a);
                        totalMoney = totalMoney - 1000 * (int)cabinets.size() - 1000;
                    }
                    break;
                case 7: // 调试：打印顾客状态
                    for (int j = 0; j < 5; j++)
                    {
                        SDL_Log("%s %d",
                            customerManager.Customers[j].GetCustomerName().c_str(),
                            customerManager.Customers[j].GetHasJoined());
                    }
                    break;
                    case 8: // 放置家具
                        isFurniturePlacing ? isFurniturePlacing = false : isFurniturePlacing = true;
                    break;
                    default:
                    break;
                }
            }
        }

        // 面包柜点击
        for (int i = 0; i < (int)cabinets.size(); i++)
        {
            if (!isSettingNewProduct && !world.IsRenderingCustomerFrame())
            {
                if (cabinets[i].isClicked(mx, my))
                {
                    currentCabinet = i;
                    cabinetFrame.SetCabinetID(i);
                    isSettingNewProduct = true;
                }
            }
        }

        // 面包柜详情框关闭按钮
        if (cabinetFrame.GetCabinetID() != -1)
        {
            if (mx >= 580 && mx <= 612 && my >= 100 && my <= 132)
            {
                cabinetFrame.SetCabinetID(-1);
                isSettingNewProduct = false;
                cabinetFrame.quit();
            }
        }
        break;
    }

    case SDL_MOUSEMOTION:
    {
        int mx = event.motion.x;
        int my = event.motion.y;
        bool cursorSet = false;

        if (isCheckingSetting)
        {
            if (mx >= 200 && mx <= 600 && my >= 0 && my <= 600)
            {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                cursorSet = true;
            }
        }

        for (int i = 0; i < (int)buttons.size(); i++)
        {
            if (isSettingNewProduct)
            {
                if (buttons[i].RUI_isHovered(mx, my))
                {
                    buttons[i].setHovered(true);
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                    cursorSet = true;
                }
                else
                {
                    buttons[i].setHovered(false);
                    buttons[i].setClicked(false);
                }
            }
        }

        for (int i = 0; i < (int)icons.Icons.size(); i++)
        {
            if (icons.Icons[i].isHovered(mx, my))
            {
                if (i <= 3 || i >= 6)
                {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                    cursorSet = true;
                }
                else
                {
                    if (world.IsReadingPage())
                    {
                        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                        cursorSet = true;
                    }
                }
            }
        }

        for (int i = 0; i < (int)cabinets.size(); i++)
        {
            if (!isCheckingSetting)
            {
                if (cabinets[i].isClicked(mx, my))
                {
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                    cursorSet = true;
                }
            }
        }

        if (cabinetFrame.GetCabinetID() != -1)
        {
            if (mx >= 580 && mx <= 612 && my >= 100 && my <= 132)
            {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                cursorSet = true;
            }
        }

        if (!cursorSet)
        {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
        break;
    }

    case SDL_KEYDOWN:
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_c:
            SceneManager.ChooseScene(RUI_SceneManager::SceneType::Create);
            break;
        case SDLK_ESCAPE:
            SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
            break;
        }
        break;
    }
    }
}
