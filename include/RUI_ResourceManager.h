#pragma once

#include <string>
#include <unordered_map>

// 前向声明 SDL 类型 —— 无需暴露 SDL 头文件给所有引用者
struct SDL_Texture;
struct SDL_Renderer;
struct Mix_Chunk;
struct Mix_Music;  // SDL_mixer 中即为 struct Mix_Music

class ResourceManager
{
public:
    static ResourceManager* instance();

    void load(SDL_Renderer* Renderer);

    SDL_Texture* FindTexture(const std::string& path);
    Mix_Music*   FindMusic(const std::string& path);
    Mix_Chunk*   FindAudio(const std::string& path);

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    static ResourceManager* Manager;

    std::unordered_map<std::string, SDL_Texture*> TexturePool;
    std::unordered_map<std::string, Mix_Music*>   MusicPool;
    std::unordered_map<std::string, Mix_Chunk*>   AudioPool;
};
