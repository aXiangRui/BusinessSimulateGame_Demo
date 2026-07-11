#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include "RUI_Icon.h"
#include "RUI_DessertManager.h"
#include "RUI_CustomerManager.h"
#include "RUI_Chair.h"
#include "RUI_Scene.h"
#include "RUI_MusicManager.h"
#include "RUI_MenuButton.h"
#include "RUI_Clock.h"
#include "RUI_ResourceManager.h"
#include "RUI_MaterialManager.h"
#include "RUI_GameWorld.h"
#include "RUI_GameSerializer.h"
#include "RUI_TimeOfDaySystem.h"
#include "RUI_Cabinet.h"
#include "RUI_MoneyDisplay.h"
#include "RUI_ChatFrame.h"
#include "RUI_TextManager.h"
#include "RUI_CheckUpdate.h"
#include "RUI_SceneManager.h"
#include "RUI_Furniture.h"
#include "RUI_PlacementManager.h"
#include "RUI_GameStage.h"
#include "RUI_PlacementTools.h"
#include "RUI_Tool.h"

extern RUI_SceneManager SceneManager;
extern MusicPlayer BackgroundMusic;
extern int WindowWidth;

class RUI_GameScene : public RUI_Scene
{
public:
    RUI_GameScene() = default;
    ~RUI_GameScene() = default;

    // ===== 场景生命周期 =====
    void onEnter() override;
    void onUpdate() override;
    void onRender(SDL_Renderer* renderer) override;
    void onInput(const SDL_Event& event, SDL_Renderer* renderer, bool& running) override;
    void onExit() override;

    // ===== 工具方法 =====
    static bool CheckRect(int sx, int ex, int sy, int ey, int mx, int my)
    {
        return mx >= sx && mx <= ex && my >= sy && my <= ey;
    }

private:
    // ===== 渲染资源 =====
    SDL_Texture* background = nullptr;
    SDL_Texture* backgroundWall = nullptr;
    SDL_Texture* nightTexture = nullptr;
    SDL_Rect backgroundRect = {0, 0, 800, 600};

    // ===== 游戏世界 =====
    GameWorld world;
    TimeOfDaySystem::State timeState;

    // ===== 实体管理器 =====
    CustomerManager customerManager;
    DessertManager dessertManager;
    TextManager textManager;
    MaterialManager materialManager;
    PlacementManager placeManager;

    // ===== 场景实体 =====
    std::vector<MenuButton> buttons;
    std::vector<Chair> chairs;
    std::vector<Desk> desks;
    std::vector<Cabinet> cabinets;
    std::vector<DeskChairSet> deskChairSets;
    std::vector<RenderCommand> renderCommands;
    Register reg;
    GameIcon icons;

    // ===== UI 组件 =====
    CabinetFrame cabinetFrame;
    MaterialFrame materialFrame;
    ChatFrame chatFrame;
    SummaryFrame summaryFrame;
    CheckUpdate checkEvent;
    UnlockFrame unlockFrame;
    MoneyDisplay moneyDisplay;

    // ===== 家具网格 =====
    std::vector<FurnitureGrid> furnitureGrids;
    BorderBox borderBox;
    // ===== 音频 =====
    MusicPlayer gameMusic;

    // ===== 时间 =====
    Clock clock;
    static const int HourDurationMs = 10000;

    // ===== 经济数据 =====
    int totalMoney = 0;
    int totalDessert = 0;
    int totalCustomers = 0;

    // ===== UI 状态 =====
    GameStage uiState = GameStage::Normal;
    int  chatDelayTime = 0;
    bool isChatShowing = false;         // 聊天框（叠加层）
    bool isSummaryShowing = false;      // 每日总结（叠加层）
    bool isMaterialFrameShowing = false;
    bool isFurniturePlacing = false;
    int  readingPage = -1;
    int  currentCabinet = -1;
};
