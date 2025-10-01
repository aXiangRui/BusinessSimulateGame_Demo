#pragma once
#include<SDL2/SDL.h>

class RUI_Scene
{
    public:
        RUI_Scene() = default;
        ~RUI_Scene() = default;

        virtual void onEnter(){};
        virtual void onUpdate(){};
        virtual void onRender(SDL_Renderer* Renderer){};
        virtual void onInput(const SDL_Event& event){};
        virtual void onExit(){};

    private:

};