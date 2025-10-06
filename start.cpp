#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>
#include<Windows.h>

#include"include/RUI_Scene.h"
#include"include/RUI_MenuScene.h"
#include"include/RUI_GameScene.h"
#include"include/RUI_SettingScene.h"
#include"include/RUI_SavingScene.h"
#include"include/RUI_SceneManager.h"

int WindowWidth = 800;
int WindowHeight = 600;
bool running = true;
SDL_Event event;
RUI_Scene* MenuScene = nullptr;
RUI_Scene* GameScene = nullptr;
RUI_Scene* SettingScene = nullptr;
RUI_Scene* SavingScene = nullptr;
RUI_SceneManager SceneManager;
const int FPS = 30;

int main(int argc, char* argv[]) 
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Window* window = SDL_CreateWindow(
        "Test",                           // 窗口标题
        SDL_WINDOWPOS_CENTERED,           // 水平位置（居中）
        SDL_WINDOWPOS_CENTERED,           // 垂直位置（居中）
        WindowWidth,WindowHeight,                   // 窗口宽高（像素）
        SDL_WINDOW_SHOWN           // 显示窗口（必选）
    );

    SDL_Renderer* Renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    MenuScene = new RUI_MenuScene();
    GameScene = new RUI_GameScene();
    SettingScene = new RUI_SettingScene();
    SavingScene = new RUI_SavingScene();

    SceneManager.SetSceneStage(MenuScene);
    while(running)
    {
        int StartTime = SDL_GetTicks();
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                }
                default:
                break;
            }
            SceneManager.onInput(event,Renderer,running);
        }
        SceneManager.onUpdate();
        SceneManager.onRender(Renderer);

        int EndTime = SDL_GetTicks();
        int DeltaTime = EndTime - StartTime;
        if(DeltaTime < 1000/FPS)
            Sleep(1000/FPS - DeltaTime);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(Renderer);
    TTF_Quit();
    SDL_Quit();

    return 0;
}