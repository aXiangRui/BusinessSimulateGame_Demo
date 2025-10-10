#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>

class MusicPlayer
{
    public:
    MusicPlayer() = default;
    ~MusicPlayer() = default;

    std::string address;
    Mix_Music* Music;

    void LoadMusic(std::string add)
    {
        address = add;       
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
        Music = Mix_LoadMUS(address.c_str());
        SDL_Log(address.c_str());
    }

    void play(int i)
    {
        Mix_PlayMusic(Music, i);
    }

    void pause()
    {
        Mix_PauseMusic();
    }

    void quit()
    {
        Mix_FreeMusic(Music);
    }

    bool isLoaded()
    {
        if(address.c_str() == nullptr)
            return false;
        return true;
    }

};