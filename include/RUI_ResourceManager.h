#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<SDL2/SDL_image.h>
#include<Windows.h>
#include<fstream>
#include<vector>
#include<string>
#include<unordered_map>
#include<algorithm>
#include<cctype>

// ResourceManager* Manager = nullptr;

class ResourceManager
{
    public:
        static ResourceManager* instance()
        {
            if(!Manager)
                Manager = new ResourceManager();
            return Manager;
        }

        void load(SDL_Renderer* Renderer)
        {
            SDL_Log("Starting resource scan...");
            // stack of directories to visit
            std::vector<std::string> dirs;
            dirs.push_back("resources");
            Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

            while(!dirs.empty())
            {
                std::string dir = dirs.back();
                dirs.pop_back();
                SDL_Log("Scanning directory: %s", dir.c_str());

                std::string search = dir + "\\*";
                WIN32_FIND_DATAA findData;
                HANDLE hFind = FindFirstFileA(search.c_str(), &findData);
                if(hFind == INVALID_HANDLE_VALUE)
                {
                    SDL_Log("  (no files) %s", dir.c_str());
                    continue;
                }

                do
                {
                    std::string name = findData.cFileName;
                    if(name == "." || name == "..")
                        continue;

                    std::string full = dir + "\\" + name;

                    if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        // push subdirectory to stack
                        dirs.push_back(full);
                        SDL_Log("  found dir: %s", full.c_str());
                        continue;
                    }

                    SDL_Log("  found file: %s", full.c_str());

                    // extension
                    std::string ext;
                    auto pos = name.rfind('.');
                    if(pos != std::string::npos)
                    {
                        ext = name.substr(pos);
                        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
                    }

                    std::string key = name.substr(0, pos);
                    // For compressed music formats, prefer loading by file path instead of RW
                    if(ext == ".mp3" || ext == ".flac" || ext == ".mod")
                    {
                        Mix_Music* mus = Mix_LoadMUS(full.c_str());
                        if(mus)
                        {
                            MusicPool[key] = mus;
                            SDL_Log("    loaded music (file): %s", key.c_str());
                        }
                        else
                        {
                            SDL_Log("    Mix_LoadMUS failed for %s: %s", full.c_str(), Mix_GetError());
                        }
                        continue;
                    }

                    // read file into memory for textures and sample audio
                    std::ifstream in(full, std::ios::binary);
                    if(!in)
                    {
                        SDL_Log("    failed open: %s", full.c_str());
                        continue;
                    }
                    std::vector<char> buf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    in.close();
                    if(buf.empty())
                    {
                        SDL_Log("    empty file: %s", full.c_str());
                        continue;
                    }

                    SDL_RWops* rw = SDL_RWFromConstMem(buf.data(), static_cast<int>(buf.size()));
                    if(!rw)
                    {
                        SDL_Log("    SDL_RWFromConstMem failed: %s", SDL_GetError());
                        continue;
                    }

                    if(ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
                    {
                        SDL_Texture* texture = IMG_LoadTexture_RW(Renderer, rw, 1);
                        if(texture)
                        {
                            TexturePool[key] = texture;
                            SDL_Log("    loaded texture: %s", key.c_str());
                        }
                        else
                        {
                            SDL_Log("    IMG_LoadTexture_RW failed for %s: %s", full.c_str(), IMG_GetError());
                        }
                    }
                    else if(ext == ".wav" || ext == ".ogg")
                    {
                        Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 1);
                        if(chunk)
                        {
                            AudioPool[key] = chunk;
                            SDL_Log("    loaded audio chunk: %s", key.c_str());
                        }
                        else
                        {
                            SDL_Log("    Mix_LoadWAV_RW failed for %s: %s", full.c_str(), Mix_GetError());
                        }
                    }
                    else if(ext == ".mp3" || ext == ".flac" || ext == ".mod")
                    {
                        Mix_Music* mus = Mix_LoadMUS_RW(rw, 1);
                        if(mus)
                        {
                            MusicPool[key] = mus;
                            SDL_Log("    loaded music: %s", key.c_str());
                        }
                        else
                        {
                            SDL_Log("    Mix_LoadMUS_RW failed for %s: %s", full.c_str(), Mix_GetError());
                        }
                    }
                    else
                    {
                        SDL_Log("    unhandled extension: %s", ext.c_str());
                        SDL_RWclose(rw);
                    }

                } while(FindNextFileA(hFind, &findData));

                FindClose(hFind);
            }

            SDL_Log("Resource scan finished. textures=%d audio=%d music=%d", (int)TexturePool.size(), (int)AudioPool.size(), (int)MusicPool.size());
        }
        Mix_Chunk* FindAudio(const std::string path)
        {
            auto it = AudioPool.find(path);
            return it != AudioPool.end() ? it->second : nullptr;
        }
        Mix_Music* FindMusic(const std::string path)
        {
            auto it = MusicPool.find(path);
            return it != MusicPool.end() ? it->second : nullptr;
        }
        SDL_Texture* FindTexture(const std::string path)
        {
            auto it = TexturePool.find(path);
            return it != TexturePool.end() ? it->second : nullptr;
        }
    private:
        ResourceManager() = default;
        ~ResourceManager() = default;
        static ResourceManager* Manager;
        std::unordered_map<std::string,Mix_Chunk*>AudioPool;
        std::unordered_map<std::string,Mix_Music*>MusicPool;
        std::unordered_map<std::string,SDL_Texture*>TexturePool;
};