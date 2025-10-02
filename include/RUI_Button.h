#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<string>
#include<vector>

class Button
{
    public:
        Button() = default;
        ~Button() = default;

        SDL_Texture* normalTexture;
        SDL_Texture* hoverTexture;
        SDL_Texture* clickTexture;

        virtual void ButtonRender(SDL_Renderer* Renderer){};
        void setPosition(int mx, int my, int mw, int mh)
        {
            desRect = {mx, my ,mw, mh};
            x = mx;
            y = my;
            w = mw;
            h = mh;
        }
        void setID(int ID)
        {
            id = ID;
        }

        void setTitle(std::string t)
        {
            text = t;
        }

        virtual void loadTexture(int id){};
        virtual void ClickApplication(){};

        bool RUI_isHovered(int mx, int my)
        {
            if(mx >= x && mx <= x + w)
            {
                if(my >= y && my <= y + h)
                {
                    return true;
                }
            }
            return false;
        }
        bool RUI_isClicked(int mx, int my, SDL_Event& event)
        {
            if(mx >= x && mx <= x + w)
            {
                if(my >= y && my <= y + h)
                {
                    if(event.type == SDL_MOUSEBUTTONDOWN)
                        return true;
                }
            }
            return false;
        }

        protected:
            int x,y,w,h;
            int id;           
            bool isHovered;
            bool isClicked;
            SDL_Rect desRect;
            std::string text;

        private:
};