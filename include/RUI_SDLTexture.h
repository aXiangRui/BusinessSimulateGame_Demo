#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <stdexcept>

// ===== RAII 纹理包装器 =====
// 自动销毁，解决 "Create 了忘记 Destroy" 的问题
class Texture
{
public:
    Texture() = default;

    // 禁止拷贝，允许移动
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept
        : texture(other.texture), rect(other.rect)
    {
        other.texture = nullptr;
        other.rect = {};
    }
    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            texture = other.texture;
            rect = other.rect;
            other.texture = nullptr;
            other.rect = {};
        }
        return *this;
    }

    ~Texture() { Destroy(); }

    // 从 Surface 创建纹理（接管 surface 内存）
    void CreateFromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
    {
        if (!surface)
            throw std::runtime_error("Texture::CreateFromSurface: surface is nullptr");
        Destroy();
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);  // surface 不再需要
        if (!texture)
            throw std::runtime_error("Texture::CreateFromSurface: SDL_CreateTextureFromSurface failed");
    }

    // 从文字创建纹理
    void CreateFromText(SDL_Renderer* renderer, TTF_Font* font,
                        const std::string& text, SDL_Color color)
    {
        if (!font) return;
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surf) return;
        CreateFromSurface(renderer, surf);
    }

    SDL_Texture* Get() const { return texture; }
    SDL_Rect GetRect() const { return rect; }
    void SetRect(SDL_Rect r) { rect = r; }
    void SetPos(int x, int y) { rect.x = x; rect.y = y; }
    int GetW() const { return rect.w; }
    int GetH() const { return rect.h; }

    explicit operator bool() const { return texture != nullptr; }

    void onRender(SDL_Renderer* renderer)
    {
        if (texture)
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

private:
    void Destroy()
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    SDL_Texture* texture = nullptr;
    SDL_Rect rect = {};
};

// ===== 缓存文字纹理 =====
// 仅在文字变化时重建，避免每帧 Create/Destroy
// 用法同 MoneyDisplay
class CachedText
{
public:
    CachedText() = default;

    void Init(TTF_Font* font, SDL_Color color, int x = 0, int y = 0)
    {
        this->font = font;
        this->color = color;
        posX = x;
        posY = y;
    }

    void SetText(SDL_Renderer* renderer, const std::string& text)
    {
        if (text == cachedText && texture)
            return;  // 没变，跳过

        cachedText = text;
        texture = Texture();
        texture.CreateFromText(renderer, font, text, color);
        texture.SetPos(posX, posY);
    }

    void SetPos(int x, int y) { posX = x; posY = y; }

    void onRender(SDL_Renderer* renderer)
    {
        texture.onRender(renderer);
    }

    explicit operator bool() const { return (bool)texture; }

private:
    TTF_Font* font = nullptr;
    SDL_Color color = {};
    int posX = 0, posY = 0;
    std::string cachedText;
    Texture texture;
};
