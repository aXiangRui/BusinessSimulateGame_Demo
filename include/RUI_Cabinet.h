#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"
#include"RUI_DessertManager.h"
#include"RUI_ProductManager.h"
#include"RUI_TextManager.h"
#include"RUI_Furniture.h"

class Cabinet
{
    public:
    Cabinet() = default;
    ~Cabinet() = default;

    static const int GRID_SIZE = 32;

    /// 将坐标吸附到最近的 32×32 格点，offset 对齐 Furniture 网格偏移
    static int SnapToGrid(int value, int offset = 0)
    {
        return ((value - offset + GRID_SIZE / 2) / GRID_SIZE) * GRID_SIZE + offset;
    }

    void InitCabinet(int id, int did, int number)
    {
        CabinetID = id;
        // 使用公式计算位置，然后吸附到格点（含偏移）
        int rawX = (CabinetID / 12) * 200 + 100 * (CabinetID % 2) + 20;
        int rawY = (CabinetID / 2 % 6) * 40 + 200;
        x = SnapToGrid(rawX, Furniture::offsetX);
        y = SnapToGrid(rawY, Furniture::offsetY);
        DessertID = did;
        dessertNumber = number;
    }

    void InitCabinet(int id, int did, int number, int x, int y)
    {
        CabinetID = id;
        // 自由位置也吸附到格点（含偏移）
        this->x = SnapToGrid(x, Furniture::offsetX);
        this->y = SnapToGrid(y, Furniture::offsetY);
        DessertID = did;
        dessertNumber = number;
    }

    void SetPosition(int newX, int newY)
    {
        x = SnapToGrid(newX, Furniture::offsetX);
        y = SnapToGrid(newY, Furniture::offsetY);
    }

    int GetX() { return x; }
    int GetY() { return y; }
    int GetRenderX() { return x - RENDER_OFFSET; }
    int GetRenderY() { return y - RENDER_OFFSET; }
    int GetCabinetID() const { return CabinetID; }
    int GetSortedY() { return y + GRID_SIZE; }

    int GetDessertID()
    {
        return DessertID;
    }

    void SetDessertID(int id)
    {
        DessertID = id;
    }

    int GetDessertNumber()
    {
        return dessertNumber;
    }

    void SetDessertNumber(int number)
    {
        dessertNumber = number;
    }

    void AddDessertNumber(int number)
    {
        SDL_Log("添加了%d搁甜点",number);
        dessertNumber += number;
    }

    // 碰撞箱为 32×32（一个格子），渲染 64×64 居中于格点
    static const int RENDER_SIZE = 64;
    static const int HITBOX_SIZE = 32;
    static const int RENDER_OFFSET = (RENDER_SIZE - HITBOX_SIZE) / 2;  // 8

    bool isClicked(int mx, int my)
    {
        // 碰撞箱即格点位置 32×32
        if(mx >= x && mx <= x + HITBOX_SIZE)
        {
            if(my >= y && my <= y + HITBOX_SIZE)
            {
                return true;
            }
        }
        return false;
    }

    void RemoveDessert(int number)
    {
        this->dessertNumber -= number;
        SDL_Log("此时的甜点数%d",dessertNumber);
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(!CabinetTexture)
            CabinetTexture = ResourceManager::instance()->FindTexture("cabinet");
        // 渲染尺寸 48×48，居中于 32×32 格点
        SDL_Rect Rect = {x - RENDER_OFFSET, y - RENDER_OFFSET, RENDER_SIZE, RENDER_SIZE};
        SDL_RenderCopy(Renderer, CabinetTexture, nullptr, &Rect);
    }
    
    private:
    int CabinetID;
    int DessertID;
    int x,y;
    SDL_Texture* CabinetTexture = nullptr;
    int dessertNumber;
};

class CabinetFrame
{
    public:
    CabinetFrame() = default;
    ~CabinetFrame() = default;

    void freeResources() {
        if (TitleSurface) {
            SDL_FreeSurface(TitleSurface);
            TitleSurface = nullptr;
        }
        if (DessertSurface) {
            SDL_FreeSurface(DessertSurface);
            DessertSurface = nullptr;
        }
        if (DessertNumberSurface) {
            SDL_FreeSurface(DessertNumberSurface);
            DessertNumberSurface = nullptr;
        }
        if (TitleTexture) {
            SDL_DestroyTexture(TitleTexture);
            TitleTexture = nullptr;
        }
        if (DessertNameTexture) {
            SDL_DestroyTexture(DessertNameTexture);
            DessertNameTexture = nullptr;
        }
        if (DessertNumberTexture) {
            SDL_DestroyTexture(DessertNumberTexture);
            DessertNumberTexture = nullptr;
        }
        if (TextFont) {
            TTF_CloseFont(TextFont);
            TextFont = nullptr;
        }
    }

    void SetCabinetID(int x)
    {
        if(IntCabinetID != x)
            freeResources();
        IntCabinetID = x;
    }

    int GetCabinetID()
    {
        return IntCabinetID;
    }

    void InitFrame()
    {
        CabinetFrameTexture = ResourceManager::instance()->FindTexture("saving");
        QuitIcon = ResourceManager::instance()->FindTexture("quiticon");
        Rect = {150,50,500,500};
        QuitRect = {580,100,32,32};
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        color = {10,10,10,255};
        IntCabinetID = -1;
        SDL_Log("已初始化");
    }

    void quit()
    {
        if (TitleSurface) {
            SDL_FreeSurface(TitleSurface);
            TitleSurface = nullptr;
        }
        if (DessertSurface) {
            SDL_FreeSurface(DessertSurface);
            DessertSurface = nullptr;
        }
        if (DessertNumberSurface) {
            SDL_FreeSurface(DessertNumberSurface);
            DessertNumberSurface = nullptr;
        }
        if (TitleTexture) {
            SDL_DestroyTexture(TitleTexture);
            TitleTexture = nullptr;
        }
        if (DessertNameTexture) {
            SDL_DestroyTexture(DessertNameTexture);
            DessertNameTexture = nullptr;
        }
        if (DessertNumberTexture) {
            SDL_DestroyTexture(DessertNumberTexture);
            DessertNumberTexture = nullptr;
        }
        if (TextFont) {
            TTF_CloseFont(TextFont);
            TextFont = nullptr;
        }
    }

    void onRender(SDL_Renderer* Renderer, Cabinet& cab, ProductManager Manager)
    {
        SDL_RenderCopy(Renderer, CabinetFrameTexture, nullptr, &Rect);
        SDL_RenderCopy(Renderer, QuitIcon, nullptr, &QuitRect);

        DessertName = Manager.GetProductName(cab.GetDessertID()) + " " + std::to_string(Manager.GetProductPrice(cab.GetDessertID())) + "元";
        CabinetID = "第" + std::to_string(IntCabinetID + 1) + "个面包柜";
        dessertNumber = "当前甜点数:" + std::to_string(cab.GetDessertNumber());

        if (TextFont == nullptr)
        {
            TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf", 36);
            if (!TextFont)
            {
                SDL_Log("CabinetFrame 字体加载失败: %s", TTF_GetError());
                return;
            }
        }

        // 每帧销毁旧纹理以刷新内容（甜点数量会变化）
        if (TitleTexture) {
            SDL_DestroyTexture(TitleTexture);
            TitleTexture = nullptr;
        }
        if (DessertNameTexture) {
            SDL_DestroyTexture(DessertNameTexture);
            DessertNameTexture = nullptr;
        }
        if (DessertNumberTexture) {
            SDL_DestroyTexture(DessertNumberTexture);
            DessertNumberTexture = nullptr;
        }

        // 释放上一帧的 Surface（防御性清理）
        if (TitleSurface) {
            SDL_FreeSurface(TitleSurface);
            TitleSurface = nullptr;
        }
        if (DessertSurface) {
            SDL_FreeSurface(DessertSurface);
            DessertSurface = nullptr;
        }
        if (DessertNumberSurface) {
            SDL_FreeSurface(DessertNumberSurface);
            DessertNumberSurface = nullptr;
        }

        // 创建新 Surface
        TitleSurface = TTF_RenderUTF8_Blended(TextFont, CabinetID.c_str(), color);
        if (!TitleSurface) {
            SDL_Log("CabinetFrame TitleSurface 创建失败: %s", SDL_GetError());
            return;
        }

        DessertSurface = TTF_RenderUTF8_Blended(TextFont, DessertName.c_str(), color);
        if (!DessertSurface) {
            SDL_Log("CabinetFrame DessertSurface 创建失败: %s", SDL_GetError());
            return;
        }

        DessertNumberSurface = TTF_RenderUTF8_Blended(TextFont, dessertNumber.c_str(), color);
        if (!DessertNumberSurface) {
            SDL_Log("CabinetFrame DessertNumberSurface 创建失败: %s", SDL_GetError());
            return;
        }

        // 从 Surface 创建 Texture
        TitleTexture = SDL_CreateTextureFromSurface(Renderer, TitleSurface);
        if (!TitleTexture) {
            SDL_Log("CabinetFrame TitleTexture 创建失败: %s", SDL_GetError());
        }

        DessertNameTexture = SDL_CreateTextureFromSurface(Renderer, DessertSurface);
        if (!DessertNameTexture) {
            SDL_Log("CabinetFrame DessertNameTexture 创建失败: %s", SDL_GetError());
        }

        DessertNumberTexture = SDL_CreateTextureFromSurface(Renderer, DessertNumberSurface);
        if (!DessertNumberTexture) {
            SDL_Log("CabinetFrame DessertNumberTexture 创建失败: %s", SDL_GetError());
        }

        int tw = TitleSurface->w; int th = TitleSurface->h;
        int dw = DessertSurface->w; int dh = DessertSurface->h;

        TitleRect = {200, 100, tw, th};
        DessertRect = {200, 400, dw, dh};
        NumberRect = {400, 300, DessertNumberSurface->w, DessertNumberSurface->h};

        if (TitleTexture)
            SDL_RenderCopy(Renderer, TitleTexture, nullptr, &TitleRect);
        if (DessertNameTexture)
            SDL_RenderCopy(Renderer, DessertNameTexture, nullptr, &DessertRect);
        if (DessertNumberTexture)
            SDL_RenderCopy(Renderer, DessertNumberTexture, nullptr, &NumberRect);

        Manager.onRender(Renderer, cab.GetDessertID());

        // 释放 Surface（纹理已创建，Surface 不再需要）
        SDL_FreeSurface(TitleSurface);
        TitleSurface = nullptr;
        SDL_FreeSurface(DessertSurface);
        DessertSurface = nullptr;
        SDL_FreeSurface(DessertNumberSurface);
        DessertNumberSurface = nullptr;
    }

    private:
    TTF_Font* TextFont = nullptr;
    SDL_Texture* CabinetFrameTexture = nullptr;
    SDL_Texture* TitleTexture = nullptr;
    SDL_Texture* DessertNameTexture = nullptr;
    SDL_Texture* QuitIcon = nullptr;
    SDL_Texture* DessertNumberTexture = nullptr;
    SDL_Rect Rect;
    SDL_Rect QuitRect;
    SDL_Rect TitleRect;
    SDL_Rect DessertRect;
    SDL_Rect NumberRect;
    SDL_Surface* TitleSurface = nullptr;
    SDL_Surface* DessertSurface = nullptr;
    SDL_Surface* DessertNumberSurface = nullptr;
    std::string DessertName;
    std::string CabinetID;
    std::string dessertNumber;
    SDL_Color color;
    int IntCabinetID;
};


class PayCharm
{
    public:
    PayCharm() = default;
    ~PayCharm() = default;

    void Init()
    {
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",24);
        Money = 0;
    }

    void SetPrice(int i)
    {
        Money = i;
        std::string price = std::to_string(Money) + "元";
        color = {10,10,10,255};
        PriceSurface = TTF_RenderUTF8_Blended(TextFont, price.c_str(), color);
        w = PriceSurface->w; h = PriceSurface->h;
        x = 350; y = 120;
        Rect = {x,y,w,h};
        StartTime = 0;
        StopTime = 0;
    }

    int GetMoney()
    {
        return Money;
    }

    void SetStartTime(int CurrentTime)
    {
        StartTime = CurrentTime;
    }

    void SetStopTime(int CurrentTime)
    {
        StopTime = CurrentTime;
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if( StopTime - StartTime < 1000)
        {
            if(PriceTexture == nullptr)
            {
                if(PriceSurface == nullptr)
                    SDL_Log("nullptr");
                PriceTexture = SDL_CreateTextureFromSurface(Renderer,PriceSurface);
            }
            SDL_RenderCopy(Renderer, PriceTexture, nullptr, &Rect);
        }
    }

    private:
    int Money;
    int StartTime;
    int StopTime;
    int x,y;
    int w,h;
    TTF_Font* TextFont;
    SDL_Rect Rect;
    SDL_Surface* PriceSurface;
    SDL_Texture* PriceTexture = nullptr;
    SDL_Color color;
};

class CustomerFrame
{
    public:
        CustomerFrame() = default;
        ~CustomerFrame() = default;

        void Init()
        {
            TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            backgroundTexture = ResourceManager::instance()->FindTexture("backgroundFrame");
            backgroundRect = { 100, 0, 600, 600};
            dessertRect = { 275, 150, 250, 250};
            dessertTexture = nullptr;
        }

        void SetCustomer(std::string customerName, std::string dessertAddress)
        {
            if(dessertAddress != "nullptr")
            {
                dessertTexture = ResourceManager::instance()->FindTexture(dessertAddress.c_str());
            }
            else
            {
                dessertTexture = nullptr;
            }
            CustomerName = customerName;
            DescribeText = "喜欢的甜品:";
            customerNameSurface = TTF_RenderUTF8_Blended( TextFont, customerName.c_str(), TextColor);
            describeTextSurface = TTF_RenderUTF8_Blended( TextFont, DescribeText.c_str(), TextColor);
            customerNameRect = { 350, 50, customerNameSurface->w, customerNameSurface->h};
            describeRect = { 350, 150, describeTextSurface->w, describeTextSurface->h};
        }

        void onRender(SDL_Renderer* Renderer)
        {
            customerNameTexture = SDL_CreateTextureFromSurface( Renderer, customerNameSurface);
            describeTexture = SDL_CreateTextureFromSurface( Renderer, describeTextSurface );
            SDL_RenderCopy( Renderer, backgroundTexture, nullptr, &backgroundRect );
            SDL_RenderCopy( Renderer, customerNameTexture, nullptr,&customerNameRect);
            SDL_RenderCopy( Renderer, describeTexture, nullptr, &describeRect);
            if(dessertTexture != nullptr)
                SDL_RenderCopy( Renderer, dessertTexture, nullptr, &dessertRect);
        }
    
    private:
        SDL_Texture* backgroundTexture;
        SDL_Texture* dessertTexture;
        SDL_Texture* describeTexture;
        SDL_Texture* customerNameTexture;
        SDL_Rect backgroundRect;
        SDL_Rect customerNameRect;
        SDL_Rect describeRect;
        SDL_Rect dessertRect;
        TTF_Font* TextFont;
        std::string CustomerName;
        std::string DescribeText;
        SDL_Surface* customerNameSurface;
        SDL_Surface* describeTextSurface;
        SDL_Color TextColor = { 10, 10, 10, 255};
};

class MaterialFrame
{
    public:
    MaterialFrame() = default;
    ~MaterialFrame() = default;

    void Init()
    {
        backgroundTexture = ResourceManager::instance()->FindTexture("saving");
        backgroundRect = {100, 0, 600, 600};
        textFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
    }

    void SetTitle(std::string name, std::string address, std::string des)
    {
        title = name;
        describe = des;
        SDL_Log("%s %s",title.c_str(),describe.c_str());
        titleSurface = TTF_RenderUTF8_Blended( textFont, title.c_str(), color);
        describeSurface = TTF_RenderUTF8_Blended_Wrapped(textFont, describe.c_str(),color,400);
        titleRect = { 150, 70, titleSurface->w, titleSurface->h};
        describeRect = { 150, 100, describeSurface->w, describeSurface->h };
        materialTexture = ResourceManager::instance()->FindTexture(address.c_str());
        materialRect = { 150, 130, 100, 100};
    }

    void onRender( SDL_Renderer* Renderer)
    {
        titleTexture = SDL_CreateTextureFromSurface(Renderer, titleSurface);
        describeTexture = SDL_CreateTextureFromSurface( Renderer, describeSurface);
        SDL_RenderCopy(Renderer, backgroundTexture, nullptr, &backgroundRect);
        SDL_RenderCopy(Renderer, titleTexture, nullptr, &titleRect);
        SDL_RenderCopy(Renderer, describeTexture, nullptr, &describeRect);
        SDL_RenderCopy(Renderer, materialTexture, nullptr, &materialRect);
    }

    private:
    SDL_Texture* backgroundTexture;
    SDL_Rect backgroundRect;
    SDL_Texture* materialTexture;
    SDL_Rect materialRect;
    TTF_Font* textFont;
    SDL_Surface* titleSurface;
    SDL_Rect titleRect;
    SDL_Texture* titleTexture;
    SDL_Surface* describeSurface;
    SDL_Rect describeRect;
    SDL_Texture* describeTexture;
    std::string title;
    std::string describe;
    SDL_Color color = {10, 10, 10, 255};
};