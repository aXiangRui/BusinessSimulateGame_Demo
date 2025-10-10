#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>

class MusicPlayer
{
    public:
    MusicPlayer() = default;
    //~MusicPlayer() = default;

    std::string address;
    Mix_Music* Music = nullptr;
    // whether this MusicPlayer owns the Mix_Music pointer (should free on quit)
    bool ownsMusic = false;

    void LoadMusic(std::string add)
    {
        address = add;       
        // Load a music from a file path and mark ownership so this player will free it
        Mix_Music* mus = Mix_LoadMUS(address.c_str());
        if(mus)
        {
            // free previous if owned
            if(ownsMusic && Music)
                Mix_FreeMusic(Music);
            Music = mus;
            ownsMusic = true;
            SDL_Log("Loaded music: %s", address.c_str());
        }
        else
        {
            SDL_Log("Mix_LoadMUS failed for %s: %s", address.c_str(), Mix_GetError());
        }
    }

    void play(int i)
    {
        if(!Music)
        {
            SDL_Log("Mix_PlayMusic failed: Music is null");
            return;
        }
        if(Mix_PlayMusic(Music, i) == -1)
            SDL_Log("Mix_PlayMusic error: %s", Mix_GetError());
    }

    void pause()
    {
        Mix_PauseMusic();
    }

    void quit()
    {
        // stop playback
        Mix_HaltMusic();
        // only free the Mix_Music if this player loaded/owns it
        if(ownsMusic && Music)
        {
            Mix_FreeMusic(Music);
            Music = nullptr;
            ownsMusic = false;
        }
        else
        {
            // do not free ResourceManager-owned music; just forget the pointer
            Music = nullptr;
        }
    }

    bool isLoaded()
    {
        return !address.empty();
    }

    bool isPrepared()
    {
        if(!Music)
            return false;
        return true;
    }

    void setMusic(Mix_Music* music)
    {
        // If we previously owned a music, free it
        if(ownsMusic && Music)
        {
            Mix_FreeMusic(Music);
            ownsMusic = false;
        }
        Music = music;
    }

    Mix_Music* getMusic() const { return Music; }

    ~MusicPlayer()
    {
        if(ownsMusic && Music)
            Mix_FreeMusic(Music);
    }

};