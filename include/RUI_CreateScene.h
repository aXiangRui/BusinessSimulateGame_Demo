#pragma once

#include"RUI_Scene.h"
#include"RUI_Icon.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_CreateEvent.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>

extern RUI_SceneManager SceneManager;

class RUI_CreateScene : public RUI_Scene
{
    public:
    RUI_CreateScene() = default;
    ~RUI_CreateScene() = default;

    MusicPlayer CreateMusic;

    enum class CreateStage
    {
        ChooseSize,
        ChooseBasicMaterial,
        Create,
        ChooseDecorationMaterial
    };

    std::vector<Button> MenuButtons;
    std::vector<RUI_Icon> Icons;
    std::vector<int> Size;

    CreateRUIEvent Event;

    void onEnter()
        {
            if(!Mix_PlayingMusic())
            {
                CreateMusic.setMusic(ResourceManager::instance()->FindMusic("createmusic"));
                CreateMusic.play(-1);
            }
            BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            MenuButton Btn0((WindowWidth-320)/2,520,320,64,"返回店中",0);
            MenuButtons.push_back(Btn0);
            CurrentStage = CreateStage::ChooseSize;

            RUI_Icon exiticon;
            exiticon.InitIcon(10,500,50,50,0,"exiticon");
            Icons.push_back(exiticon);

            for(int i = 0; i < 4; i++)
            {
                Size.push_back( 16* (i+1) );
            }
            Event.load();
        }
        void onUpdate()
        {
            Event.SetStage(StageInt());
            //SDL_Log("更新设置场景");
            switch(CurrentStage)
            {
                case CreateStage::ChooseSize:{Event.ChooseSize(Size);break;}
                case CreateStage::ChooseBasicMaterial:
                {
                    break;
                }
                case CreateStage::Create:
                {
                    break;
                }
                case CreateStage::ChooseDecorationMaterial:
                {
                    break;
                }
                default:
                break;
            }
        }
        void onRender(SDL_Renderer* Renderer)
        {
            SDL_SetRenderDrawColor(Renderer,10,10,10,255);
            SDL_RenderClear(Renderer);
            SDL_RenderCopy(Renderer, BackgroundTexture, nullptr, &BackgroundRect);

            for(int i = 0; i < Icons.size(); i++)
            {
                Icons[i].onRender(Renderer);
            }

            Event.onRender(Renderer);

            SDL_RenderPresent(Renderer);
        }
        void onInput(const SDL_Event& event,SDL_Renderer* Renderer, bool& running)
        {
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                int mx = event.button.x;
                int my = event.button.y;
                for(int i = 0; i <= Icons.size(); i++)
                {
                    if(Icons[i].isClicked(mx,my))
                    {
                        switch(i)
                        {
                            case 0:
                            {
                                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                break;
            }
            default:
            break;
        }
            // if(event.type == SDL_MOUSEBUTTONDOWN)
            // {
            //     SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
            // }    
        }
        void onExit()
        {
            CreateMusic.quit();
            SDL_Log("退出创造场景");
        }

        int StageInt()
        {
            switch(CurrentStage)
            {
                case CreateStage::ChooseSize:{return 1;break;}
                case CreateStage::ChooseBasicMaterial:{return 2; break;}
                case CreateStage::Create:{return 3;break;}
                case CreateStage::ChooseDecorationMaterial:{return 4;break;}
                default:{return 0;break;}
            }
        }

    private:
        SDL_Texture* BackgroundTexture;
        SDL_Rect BackgroundRect = {0,0,800,600};
        CreateStage CurrentStage;
};