#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include"RUI_ResourceManager.h"
#include"RUI_DessertManager.h"
#include"RUI_ProductManager.h"
#include"RUI_TextManager.h"

class Cabinet
{
    public:
    Cabinet() = default;
    ~Cabinet() = default;

    void InitCabinet(int id,int did,int number)
    {
        CabinetID = id;
        x = (CabinetID / 12) * 200 + 100 * (CabinetID % 2) + 20;
        y = (CabinetID / 2 % 6) * 40 + 200; 
        DessertID = did;
        dessertNumber = number;
    }

    int GetX()
    {
        return x;
    }

    int GetY()
    {
        return y;
    }

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

    bool isClicked(int mx, int my)
    {
        if(mx >= x && mx <= x + 64)
        {
            if(my >= y && my <= y + 64)
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
        SDL_Rect Rect = {x,y,64,64};
        SDL_RenderCopy(Renderer,CabinetTexture,nullptr,&Rect);
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
        if (TitleTexture) {
            SDL_DestroyTexture(TitleTexture);
            TitleTexture = nullptr;
        }
        if (DessertNameTexture) {
            SDL_DestroyTexture(DessertNameTexture);
            DessertNameTexture = nullptr;
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
        SDL_FreeSurface(TitleSurface);
        SDL_FreeSurface(DessertSurface);
        TitleSurface = nullptr;
        DessertSurface = nullptr;
        SDL_DestroyTexture(TitleTexture);
        SDL_DestroyTexture(DessertNameTexture);
        TitleTexture = nullptr;
        DessertNameTexture = nullptr;
        if(TextFont)
        {
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

        if(TextFont == nullptr)
        {
            TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
            if (!TextFont) 
            {
            SDL_Log("字体加载失败: %s", TTF_GetError());
            return; 
            }
        }
            
        if(TitleSurface == nullptr)
            TitleSurface = TTF_RenderUTF8_Blended(TextFont, CabinetID.c_str(), color);
        if(DessertSurface == nullptr)
            DessertSurface = TTF_RenderUTF8_Blended(TextFont, DessertName.c_str(), color);
        if(DessertNumberSurface == nullptr)
            DessertNumberSurface = TTF_RenderUTF8_Blended(TextFont, dessertNumber.c_str(), color);

        if(TitleTexture == nullptr)
            TitleTexture = SDL_CreateTextureFromSurface(Renderer,TitleSurface);
        if(DessertNameTexture == nullptr)
            DessertNameTexture = SDL_CreateTextureFromSurface(Renderer,DessertSurface);
        if(DessertNumberTexture == nullptr)
            DessertNumberTexture = SDL_CreateTextureFromSurface(Renderer, DessertNumberSurface);

        int tw = TitleSurface->w; int th = TitleSurface->h;
        int dw = DessertSurface->w; int dh = DessertSurface->h;

        TitleRect = {200,100,tw,th};
        DessertRect = {200, 400, dw, dh};
        NumberRect = {400, 300, DessertNumberSurface->w, DessertNumberSurface->h};

        SDL_RenderCopy(Renderer, TitleTexture, nullptr, &TitleRect);
        SDL_RenderCopy(Renderer, DessertNameTexture, nullptr, &DessertRect);
        SDL_RenderCopy(Renderer, DessertNumberTexture, nullptr, &NumberRect);
        Manager.onRender(Renderer,cab.GetDessertID());
        SDL_FreeSurface(TitleSurface);
        SDL_FreeSurface(DessertNumberSurface);
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