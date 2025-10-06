#pragma once

#include<SDL2/SDL.h>
#include<string>

class RUI_Icon
{
    RUI_Icon() = default;
    ~RUI_Icon() = default;

    virtual void IconRender(SDL_Renderer* Renderer,std::string);
    virtual void IconHoverRender(SDL_Renderer* Renderer,std::string);
    virtual void IconClickRender(SDL_Renderer* Renderer,std::string);

    void InitIcon(int mx, int my, int mw, int mh, int i, std::string add)
    {
        x = mx;
        y = my;
        w = mw;
        h = mh;
        Rect = {x,y,w,h};
        id = i;
        address = add;
    }

    bool isHovered(SDL_Event& Event)
    {
        int mx = Event.motion.x;
        int my = Event.motion.y;
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    bool isClicked(SDL_Event& Event)
    {
        int mx = Event.motion.x;
        int my = Event.motion.y;
        if(mx>=x && mx<=x+w)
        {
            if(my >= y && my <= y + h)
            {
                return true;
            }
        }
        return false;
    }

    private:
    int x,y,w,h;
    SDL_Rect Rect;
    int id;
    std::string address;

};