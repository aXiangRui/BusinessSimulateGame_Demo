#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include"RUI_ResourceManager.h"

class ChatFrame
{
    public:
    ChatFrame() = default;
    ~ChatFrame() = default;

    void setID(int id)
    {
        ChatFrameID = id;
    }

    void setTitle(std::string title)
    {
        Title = title;
    }

    void setContent(std::string content)
    {
        Content = content;
    }

    int getID()
    {
        return ChatFrameID;
    }

    std::string getTitle()
    {
        return Title; 
    }

    std::string getContent()
    {
        return Content;
    }

    void RenderFrame(SDL_Renderer* Renderer)
    {
        FrameTexture = ResourceManager::instance()->FindTexture("ChatFrame");
        SDL_Rect rect = {150,50,500,500};
        SDL_RenderCopy(Renderer,FrameTexture,nullptr,&rect);
    }

    private:
    int ChatFrameID;
    std::string Title;
    std::string Content;
    SDL_Texture* FrameTexture;
};