#pragma once

#include"RUI_Scene.h"
#include<SDL2/SDL.h>

extern RUI_Scene* MenuScene;
extern RUI_Scene* GameScene;
extern RUI_Scene* SettingScene;

class RUI_SceneManager
{
    public:

    RUI_SceneManager() = default;
    ~RUI_SceneManager() = default;

    enum class SceneType
    {
        Menu,
        Game,
        Setting
    };

    void SetSceneStage(RUI_Scene* scene)
    {
        CurrentScene = scene;
        CurrentScene->onEnter();
    }

    void ChooseScene(SceneType scene)
    {
        CurrentScene->onExit();
        switch (scene)
        {
        case SceneType::Menu:
            CurrentScene = MenuScene;
            break;
        case SceneType::Game:
            CurrentScene = GameScene;
            break;
        case SceneType::Setting:
            CurrentScene = SettingScene;
            break;
        default:
            break;
        }
        CurrentScene->onEnter();
    }
    void onUpdate()
        {
            CurrentScene->onUpdate();
        }
    void onRender(SDL_Renderer* Renderer)
        {
            CurrentScene->onRender(Renderer);
        }
    void onInput(const SDL_Event& event,SDL_Renderer* Renderer,bool& running)
        {
            CurrentScene->onInput(event,Renderer,running);
        }

    private:
        RUI_Scene* CurrentScene = nullptr;
};