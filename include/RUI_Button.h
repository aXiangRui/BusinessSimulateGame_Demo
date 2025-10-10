#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
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
        SDL_Texture* TextTexture;

        SDL_Rect ButtonRect;
        SDL_Rect TextRect;

        SDL_Color TextColor;

        TTF_Font *TextFont;

    virtual void ButtonRender(SDL_Renderer* Renderer){};
    virtual void HoveredButtonRender(SDL_Renderer* Renderer){};
    virtual void ClickedButtonRender(SDL_Renderer* Renderer){};

    // state accessors
    void setHovered(bool v) { isHovered = v; }
    void setClicked(bool v) { isClicked = v; }
    bool getHovered() const { return isHovered; }
    bool getClicked() const { return isClicked; }
        

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

        bool getClicked()
        {
            return isClicked;
        }

        bool getHovered()
        {
            return isHovered;
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
        bool RUI_isClicked(int mx, int my)
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

        void setTextColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        {
            TextColor = {r,g,b,a};
        }

        protected:
            int x,y,w,h;
            int id;           
            bool isHovered = false;
            bool isClicked = false;
            SDL_Rect desRect;
            std::string text;

        private:
};