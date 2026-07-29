#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_CreateEvent.h"
#include"RUI_CreateMode.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>

extern RUI_SceneManager SceneManager;

// ============================================================================
// 制作场景 —— 入口选模式，然后委托给对应子模式
// ============================================================================
class RUI_CreateScene : public RUI_Scene
{
    public:
    RUI_CreateScene() = default;
    ~RUI_CreateScene() = default;

    enum class CreateMode
    {
        ChooseMode,
        Classic,
        DIY
    };

    void onEnter()
    {
        currentMode = CreateMode::ChooseMode;
        BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

        MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回店中",0);
        Btn0.SetOnClick([this]()
        {
            onExit();
            SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
        });
        buttons.push_back(Btn0);
    }

    void onUpdate()
    {
        switch(currentMode)
        {
            case CreateMode::ChooseMode:
                break;
            case CreateMode::Classic:
                classicMode.onUpdate();
                if(classicMode.GetWhetherBack())
                {
                    onExit();
                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                }
                break;
            case CreateMode::DIY:
                diyMode.onUpdate();
                if(diyMode.GetWhetherBack())
                {
                    onExit();
                    SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                }
                break;
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        SDL_SetRenderDrawColor(Renderer,10,10,10,255);
        SDL_RenderClear(Renderer);
        SDL_RenderCopy(Renderer, BackgroundTexture, nullptr, &BackgroundRect);

        switch(currentMode)
        {
            case CreateMode::ChooseMode:
                RenderModeSelection(Renderer);
                break;
            case CreateMode::Classic:
                classicMode.onRender(Renderer);
                return;
            case CreateMode::DIY:
                diyMode.onRender(Renderer);
                return;
        }

        SDL_RenderPresent(Renderer);
    }

    void onInput(const SDL_Event& event, SDL_Renderer* Renderer, bool& running)
    {
        if(event.type != SDL_MOUSEBUTTONDOWN)
        {
            if(currentMode == CreateMode::Classic)
                classicMode.onInput(event);
            else if(currentMode == CreateMode::DIY)
                diyMode.onInput(event);
            return;
        }

        int mx = event.button.x;
        int my = event.button.y;

        for(auto& btn : buttons)
        {
            if(btn.RUI_isClicked(mx,my))
                btn.ClickApplication();
        }

        switch(currentMode)
        {
            case CreateMode::ChooseMode:
            {
                if(mx >= 100 && mx <= 350 && my >= 230 && my <= 310)
                {
                    classicMode.onEnter();
                    currentMode = CreateMode::Classic;
                }
                else if(mx >= 450 && mx <= 700 && my >= 230 && my <= 310)
                {
                    diyMode.onEnter();
                    currentMode = CreateMode::DIY;
                }
                break;
            }
            case CreateMode::Classic:
                classicMode.onInput(event);
                break;
            case CreateMode::DIY:
                diyMode.onInput(event);
                break;
        }
    }

    void onExit()
    {
        switch(currentMode)
        {
            case CreateMode::Classic: classicMode.onExit(); break;
            case CreateMode::DIY:     diyMode.onExit();     break;
            default: break;
        }
        buttons.clear();
        SDL_Log("退出创造场景");
    }

    private:
        void RenderModeSelection(SDL_Renderer* Renderer)
        {
            SDL_Rect classicBtn = {100, 230, 250, 80};
            SDL_SetRenderDrawColor(Renderer, 180, 120, 60, 255);
            SDL_RenderFillRect(Renderer, &classicBtn);

            SDL_Rect diyBtn = {450, 230, 250, 80};
            SDL_SetRenderDrawColor(Renderer, 200, 100, 150, 255);
            SDL_RenderFillRect(Renderer, &diyBtn);
        }

        SDL_Texture* BackgroundTexture = nullptr;
        SDL_Rect BackgroundRect = {0,0,800,600};
        std::vector<MenuButton> buttons;
        CreateMode currentMode = CreateMode::ChooseMode;
        ClassicMode classicMode;
        DIYMode diyMode;
};
