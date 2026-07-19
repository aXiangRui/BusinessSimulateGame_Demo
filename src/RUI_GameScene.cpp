#include "../include/RUI_GameScene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>

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

    SDL_Log("Current Hour: %d", clock.ReturnAllHour());

    // 加载家具位置（统一数据源，替代 Cabinet.rui 中的 x/y）
    placeManager.Load();

    // 用 PlacementManager 覆写 Cabinet 位置
    auto cabEntries = placeManager.GetByType(FurnitureType::Cabinet);
    for (auto& entry : cabEntries)
    {
        for (auto& cab : cabinets)
        {
            if (cab.GetCabinetID() == entry.id)
            {
                cab.SetPosition(entry.PixelX(), entry.PixelY());
                break;
            }
        }
    }

    // 初始化游戏世界
    world.OnEnter();

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
    // col (i) = y 方向，row (j) = x 方向
    // 厨房区：col >= 12（屏幕下方），顾客不可通行
    furnitureGrids.clear();
    int id = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 25; j++)
        {
            if( i < 13 || j < 13)
            {
                FurnitureGrid grid;
                grid.InitFurnitureGrid({i, j}, FurnitureType::None, PlacementType::Showing, id++);
                if(j < 13 && i < 5)
                {
                    grid.SetPlacement(PlacementType::Kitchen);
                }
                else if( j < 13)
                {
                    grid.SetPlacement(PlacementType::Showing);
                }
                else
                {
                    grid.SetPlacement(PlacementType::Eating);
                }
                furnitureGrids.push_back(grid);
            }
        }
    }
    // 统一从 PlacementManager 标记所有家具网格（替代原来分散的绑定）
    for (auto& entry : placeManager.GetEntries())
    {
        for (auto& gridCell : furnitureGrids)
        {
            if (gridCell.GetX() == entry.PixelX() &&
                gridCell.GetY() == entry.PixelY())
            {
                gridCell.SetType(entry.type);
                gridCell.SetID(entry.id);
                break;
            }
        }
    }


    // 补充：从 PlacementManager 创建 deskChairSets 中没有的新桌椅（新放置系统）
    auto pmDesks  = placeManager.GetByType(FurnitureType::Desk);
    auto pmChairs = placeManager.GetByType(FurnitureType::Chair);
    for (auto& entry : pmDesks)
    {
        bool exists = false;
        for (auto& d : desks)
            if (d.GetDeskID() == entry.id) { exists = true; break; }
        if (!exists)
        {
            Desk desk;
            desk.initDesk(entry.id, entry.PixelX(), entry.PixelY());
            desks.push_back(desk);
        }
    }
    for (auto& entry : pmChairs)
    {
        bool exists = false;
        for (auto& c : chairs)
            if (c.GetChairID() == entry.id) { exists = true; break; }
        if (!exists)
        {
            Chair chair;
            chair.InitChair(entry.id, entry.PixelX(), entry.PixelY(), entry.flipped);
            chairs.push_back(chair);
        }
    }

    borderBox.InitBorderBox(0, 0, 200, 500, "borderBox");
    borderBox.SetFurnitureTemplates(FurnitureTemplates,
        sizeof(FurnitureTemplates) / sizeof(FurnitureTemplates[0]));

    // 初始化时间和时钟
    timeState = {};
    timeState.lastTime = SDL_GetTicks();
    world.SetClock(clock);
    clock.SetStartTime(world.GetClockTime());
    materialFrame.Init();

    // 重置 UI 状态
    uiState = GameStage::Normal;
    readingPage = -1;
    currentCabinet = -1;
    chatDelayTime = 0;
    isChatShowing = false;
    isSummaryShowing = false;
    isMaterialFrameShowing = false;

    moneyDisplay.Init(10, 60);

    icons.Icons[1].SetOnClick([]()
    {
        SceneManager.ChooseScene(RUI_SceneManager::SceneType::Create);
    });
    icons.Icons[2].SetOnClick([](){
        SceneManager.ChooseScene(RUI_SceneManager::SceneType::Menu);
    });
    icons.Icons[3].SetOnClick([this](){
        if (uiState != GameStage::ReadProduct)
        {
            uiState = GameStage::ReadProduct;
            readingPage = 0;
        }
        else
        {
            uiState = GameStage::Normal;
            readingPage = -1;
        }
    });
    icons.Icons[4].SetOnClick([this](){
        if (uiState == GameStage::ReadProduct || uiState == GameStage::CheckingProduct)
        {
            readingPage++;
            int maxPage = world.GetProductCount() / 6;
            if (readingPage > maxPage)
            {
                readingPage = maxPage;
            }
        }
    });
    icons.Icons[5].SetOnClick([this](){
        if (uiState == GameStage::ReadProduct || uiState == GameStage::CheckingProduct)
        {
            readingPage--;
            if (readingPage < 0)
            {
                readingPage = 0;
            }
        }
    });
    icons.Icons[6].SetOnClick([this](){
        if ((int)cabinets.size() < 24)
        {
            Cabinet a;
            a.InitCabinet((int)cabinets.size(), 0, 0);
            cabinets.push_back(a);
            totalMoney = totalMoney - 1000 * (int)cabinets.size() - 1000;
        }
    });
    icons.Icons[7].SetOnClick([this](){
        for (int j = 0; j < 5; j++)
        {
            SDL_Log("%s %d",
                customerManager.Customers[j].GetCustomerName().c_str(),
                customerManager.Customers[j].GetHasJoined());
        }
    });
    icons.Icons[8].SetOnClick([this](){
        isFurniturePlacing ? isFurniturePlacing = false : isFurniturePlacing = true;
        if(!isFurniturePlacing)
        {
            borderBox.CancelPlace();
        }
    });
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
            clock.ReturnHour(),
            furnitureGrids);
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
    moneyDisplay.SetMoney(totalMoney);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, nullptr, &backgroundRect);
    moneyDisplay.onRender(renderer);

    // 渲染游戏世界实体
    world.RenderCooks(renderer);
    world.RenderSmallCakes(renderer);
    world.RenderServers(renderer);

    reg.onRender(renderer);

    renderCommands.clear();

    for(int i = 0; i < (int)cabinets.size(); i++)
    {
        renderCommands.push_back({cabinets[i].GetSortedY(), 1, i});
    }

    for(int i = 0 ; i < (int)world.GetCustomerCount(); i++)
    {
        renderCommands.push_back({world.GetCustomers()[i].GetSortedY(), 2, i});
    }

    for(int i = 0; i < (int)chairs.size(); i++)
    {
        renderCommands.push_back({chairs[i].GetRenderY(), 3, i});
    }

    for(int i = 0; i < (int)desks.size(); i++)
    {
        renderCommands.push_back({desks[i].GetRenderY(), 4, i});
    }

    std::sort(renderCommands.begin(), renderCommands.end(), [](const RenderCommand& a, const RenderCommand& b)
    {
        return a.y < b.y;
    });

    for(auto& r : renderCommands)
    {
        if(r.type == 1)
        {
            cabinets[r.index].onRender(renderer);
        }
        else if( r.type == 2)
        {
            world.RenderCustomer(renderer, r.index);
        }
        else if( r.type == 3)
        {
            chairs[r.index].onRender(renderer);
        }
        else if( r.type == 4)
        {
            desks[r.index].onRender(renderer);
        }
    }

    clock.RenderHour(renderer);
        // 黑夜遮罩
    SDL_SetTextureAlphaMod(nightTexture, timeState.currentAlpha);
    SDL_RenderCopy(renderer, nightTexture, nullptr, &backgroundRect);

    // 渲染放置框
    if(isFurniturePlacing)
    {
        for( int i = 0; i < furnitureGrids.size(); i++)
        {
            furnitureGrids[i].onRender(renderer);
        }

        // 高亮光标悬停的格子（亮黄框）
        if (cursorGridCol >= 0 && cursorGridRow >= 0)
        {
            SDL_Rect hlRect = {
                cursorGridRow * Furniture::FurnitureWidth + Furniture::offsetX,
                cursorGridCol * Furniture::FurnitureHeight + Furniture::offsetY,
                Furniture::FurnitureWidth,
                Furniture::FurnitureHeight
            };
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 60);
            SDL_RenderFillRect(renderer, &hlRect);      // 半透明黄填充
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 200);
            SDL_RenderDrawRect(renderer, &hlRect);      // 亮黄边框
        }
    }

    // 图标（翻页按钮在 ReadProduct / CheckingProduct 状态下显示）
    bool showPageNav = (uiState == GameStage::ReadProduct || uiState == GameStage::CheckingProduct);
    icons.onRender(renderer, showPageNav);
    borderBox.onRender(renderer);

    // 面包柜详情框
    if (cabinetFrame.GetCabinetID() != -1)
    {
        world.RenderCabinetFrame(renderer, cabinetFrame, cabinets);
    }

    // 产品查看页
    if (uiState == GameStage::ReadProduct)
    {
        world.RenderProductPage(renderer, readingPage, dessertManager, materialManager);
    }

    // 按钮
    for (int i = 0; i < (int)buttons.size(); i++)
    {
        if (uiState == GameStage::SettingNewProduct)
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
    if (uiState == GameStage::CheckingProduct)
    {
        world.RenderSettingProduct(renderer, readingPage, dessertManager, materialManager);
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

    // 家具位置统一存档
    placeManager.Save();

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
    moneyDisplay.Quit();
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

        // 右键删除家具（放置模式下）
        if (isFurniturePlacing && event.button.button == SDL_BUTTON_RIGHT)
        {
            GridPos pos;
            pos.row = (mx + Furniture::FurnitureWidth / 2) / Furniture::FurnitureWidth;
            pos.col = (my - Furniture::offsetY + Furniture::FurnitureHeight / 2) / Furniture::FurnitureHeight;

            FurnitureGrid* targetCell = nullptr;
            for (auto& cell : furnitureGrids)
            {
                if (cell.GetPos().col == pos.col && cell.GetPos().row == pos.row)
                {
                    targetCell = &cell;
                    break;
                }
            }
            if (targetCell && targetCell->GetType() != FurnitureType::None)
            {
                FurnitureType type = targetCell->GetType();
                int id = targetCell->GetID();

                switch (type)
                {
                case FurnitureType::Cabinet:
                    for (size_t i = 0; i < cabinets.size(); i++)
                        if (cabinets[i].GetCabinetID() == id)
                            { cabinets.erase(cabinets.begin() + i); break; }
                    break;
                case FurnitureType::Desk:
                    for (size_t i = 0; i < desks.size(); i++)
                        if (desks[i].GetDeskID() == id)
                            { desks.erase(desks.begin() + i); break; }
                    break;
                case FurnitureType::Chair:
                    for (size_t i = 0; i < chairs.size(); i++)
                        if (chairs[i].GetChairID() == id)
                            { chairs.erase(chairs.begin() + i); break; }
                    break;
                default: break;
                }

                placeManager.TryRemove(id, type);
                targetCell->InitFurnitureGrid(pos, FurnitureType::None,
                    targetCell->GetPlacement(), -1);

                SDL_Log("移除家具: type=%d id=%d", (int)type, id);
            }
        }

        // 放置家具（先判断放置，再判断选模板——避免同一帧选了模板又立刻放置）
        if (borderBox.IsPlacing() && !borderBox.IsInPanel(mx, my))
        {
            const FurnitureTemplate* tpl = borderBox.GetSelectedTemplate();
            if (tpl && !tpl->tiles.empty() && totalMoney >= tpl->cost)
            {
                // 光标→格点：用格子中心偏移(+16)，不用实体中心(+32)
                GridPos anchor;
                anchor.row = (mx + Furniture::FurnitureWidth / 2) / Furniture::FurnitureWidth;
                anchor.col = (my - Furniture::offsetY + Furniture::FurnitureHeight / 2) / Furniture::FurnitureHeight;

                // 先校验所有子格是否都可放置
                bool allValid = true;
                for (const auto& tile : tpl->tiles)
                {
                    GridPos tilePos;
                    tilePos.col = anchor.col + tile.offsetCol;
                    tilePos.row = anchor.row + tile.offsetRow;

                    FurnitureGrid* cell = nullptr;
                    for (auto& c : furnitureGrids)
                    {
                        if (c.GetPos().col == tilePos.col && c.GetPos().row == tilePos.row)
                            { cell = &c; break; }
                    }
                    if (!cell || cell->GetType() != FurnitureType::None)
                        { allValid = false; break; }

                    PlacementType zone = cell->GetPlacement();
                    switch (tile.type)
                    {
                    case FurnitureType::Cabinet:
                        if (zone != PlacementType::Showing) allValid = false; break;
                    case FurnitureType::Chair:
                    case FurnitureType::Desk:
                        if (zone != PlacementType::Eating)  allValid = false; break;
                    default: break;
                    }
                    if (!allValid) break;
                }

                if (allValid)
                {
                    totalMoney -= tpl->cost;
                    for (const auto& tile : tpl->tiles)
                    {
                        GridPos tilePos;
                        tilePos.col = anchor.col + tile.offsetCol;
                        tilePos.row = anchor.row + tile.offsetRow;
                        int px = GridToPixelX(tilePos.row);
                        int py = GridToPixelY(tilePos.col);
                        int newId = placeManager.GetNextId(tile.type);

                        switch (tile.type)
                        {
                        case FurnitureType::Desk:
                        {
                            Desk desk;
                            desk.initDesk(newId, px, py);
                            desks.push_back(desk);
                            break;
                        }
                        case FurnitureType::Chair:
                        {
                            Chair chair;
                            chair.InitChair(newId, px, py, tile.flipped);
                            chairs.push_back(chair);
                            break;
                        }
                        case FurnitureType::Cabinet:
                        {
                            Cabinet cabinet;
                            cabinet.InitCabinet(newId, 0, 0, px, py);
                            cabinets.push_back(cabinet);
                            break;
                        }
                        default: break;
                        }

                        // 标记网格 + 持久化
                        for (auto& c : furnitureGrids)
                        {
                            if (c.GetPos().col == tilePos.col && c.GetPos().row == tilePos.row)
                            {
                                c.SetType(tile.type);
                                c.SetID(newId);
                                break;
                            }
                        }
                        placeManager.TryPlace(newId, tile.type, tilePos, tile.flipped,
                            tile.type == FurnitureType::Cabinet ? PlacementType::Showing : PlacementType::Eating);
                    }
                }
            }
            borderBox.ConfirmPlace();
            break;  // 放置模式下不触发其他点击逻辑
        }

        borderBox.onClickInput(isFurniturePlacing, mx, my);

        if (uiState == GameStage::CheckingProduct)
        {
            if (mx >= 200 && mx <= 600 && my >= 0 && my <= 600)
            {
                int offset = ((mx > 400) ? 3 : 0) + (my / 200);
                cabinets[currentCabinet].SetDessertID(readingPage * 6 + offset);
                uiState = GameStage::SettingNewProduct;
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
            if (uiState == GameStage::SettingNewProduct)
            {
                if (buttons[i].RUI_isClicked(mx, my))
                {
                    buttons[i].setClicked(true);
                    if (i == 0)
                    {
                        uiState = GameStage::CheckingProduct;
                        readingPage = 0;
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
                icons.Icons[i].ClickApplication();
            }
        }

        // 面包柜点击
        for (int i = 0; i < (int)cabinets.size(); i++)
        {
            if (uiState != GameStage::SettingNewProduct && !world.IsRenderingCustomerFrame())
            {
                if (cabinets[i].isClicked(mx, my))
                {
                    currentCabinet = i;
                    cabinetFrame.SetCabinetID(i);
                    uiState = GameStage::SettingNewProduct;
                }
            }
        }

        // 面包柜详情框关闭按钮
        if (cabinetFrame.GetCabinetID() != -1)
        {
            if (mx >= 580 && mx <= 612 && my >= 100 && my <= 132)
            {
                cabinetFrame.SetCabinetID(-1);
                uiState = GameStage::Normal;
                cabinetFrame.quit();
            }

            // 拆除按钮
            SDL_Rect delR = cabinetFrame.GetDeleteRect();
            if (mx >= delR.x && mx <= delR.x + delR.w &&
                my >= delR.y && my <= delR.y + delR.h)
            {
                int idx = currentCabinet;  // 向量索引，不是家具实际 ID
                int realId = cabinets[idx].GetCabinetID();
                // 重置网格
                for (auto& cell : furnitureGrids)
                {
                    if (cell.GetID() == realId && cell.GetType() == FurnitureType::Cabinet)
                    {
                        GridPos pos = cell.GetPos();
                        cell.InitFurnitureGrid(pos, FurnitureType::None, cell.GetPlacement(), -1);
                        break;
                    }
                }
                // 从存档和实体向量移除
                placeManager.TryRemove(realId, FurnitureType::Cabinet);
                cabinets.erase(cabinets.begin() + idx);
                cabinetFrame.SetCabinetID(-1);
                uiState = GameStage::Normal;
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

        // 缓存光标，避免每次鼠标移动都创建新光标（内存泄漏 + 性能损耗）
        static SDL_Cursor* handCursor  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        static SDL_Cursor* arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

        borderBox.onMotionInput( isFurniturePlacing, mx, my );

        // 更新光标悬停格点（用于高亮）
        if (isFurniturePlacing)
        {
            cursorGridRow = (mx + Furniture::FurnitureWidth / 2) / Furniture::FurnitureWidth;
            cursorGridCol = (my - Furniture::offsetY + Furniture::FurnitureHeight / 2) / Furniture::FurnitureHeight;
        }
        else
        {
            cursorGridCol = -1;
            cursorGridRow = -1;
        }

        // 放置预览：吸附到网格 + 半透明 + 绿/红
        if (borderBox.IsPlacing() && !borderBox.IsInPanel(mx, my))
        {
            // 光标→格点：用格子中心偏移，与放置逻辑一致
            GridPos anchor;
            anchor.row = (mx + Furniture::FurnitureWidth / 2) / Furniture::FurnitureWidth;
            anchor.col = (my - Furniture::offsetY + Furniture::FurnitureHeight / 2) / Furniture::FurnitureHeight;
            bool valid = false;

            const FurnitureTemplate* ptpl = borderBox.GetSelectedTemplate();
            if (ptpl && !ptpl->tiles.empty())
            {
                valid = true;
                for (const auto& tile : ptpl->tiles)
                {
                    GridPos tilePos;
                    tilePos.col = anchor.col + tile.offsetCol;
                    tilePos.row = anchor.row + tile.offsetRow;

                    bool cellFound = false;
                    for (auto& cell : furnitureGrids)
                    {
                        if (cell.GetPos().col == tilePos.col && cell.GetPos().row == tilePos.row)
                        {
                            cellFound = true;
                            if (cell.GetType() != FurnitureType::None)
                                valid = false;
                            else
                            {
                                PlacementType zone = cell.GetPlacement();
                                switch (tile.type)
                                {
                                case FurnitureType::Cabinet:
                                    if (zone != PlacementType::Showing) valid = false; break;
                                case FurnitureType::Chair:
                                case FurnitureType::Desk:
                                    if (zone != PlacementType::Eating)  valid = false; break;
                                default: break;
                                }
                            }
                            break;
                        }
                    }
                    if (!cellFound) valid = false;
                    if (!valid) break;
                }
            }

            borderBox.SetPreviewPos(anchor, valid);
        }

        if (uiState == GameStage::CheckingProduct)
        {
            if (mx >= 200 && mx <= 600 && my >= 0 && my <= 600)
            {
                SDL_SetCursor(handCursor);
                cursorSet = true;
            }
        }

        for (int i = 0; i < (int)buttons.size(); i++)
        {
            if (uiState == GameStage::SettingNewProduct)
            {
                if (buttons[i].RUI_isHovered(mx, my))
                {
                    buttons[i].setHovered(true);
                    SDL_SetCursor(handCursor);
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
                    SDL_SetCursor(handCursor);
                    cursorSet = true;
                }
                else
                {
                    if (uiState == GameStage::ReadProduct || uiState == GameStage::CheckingProduct)
                    {
                        SDL_SetCursor(handCursor);
                        cursorSet = true;
                    }
                }
            }
        }

        for (int i = 0; i < (int)cabinets.size(); i++)
        {
            if (uiState != GameStage::CheckingProduct)
            {
                if (cabinets[i].isClicked(mx, my))
                {
                    SDL_SetCursor(handCursor);
                    cursorSet = true;
                }
            }
        }

        if (cabinetFrame.GetCabinetID() != -1)
        {
            if (mx >= 580 && mx <= 612 && my >= 100 && my <= 132)
            {
                SDL_SetCursor(handCursor);
                cursorSet = true;
            }
        }

        if (!cursorSet)
        {
            SDL_SetCursor(arrowCursor);
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
