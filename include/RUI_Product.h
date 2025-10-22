#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include"RUI_BaseMaterial.h"
#include"RUI_MaterialManager.h"
#include"RUI_DessertManager.h"
#include"RUI_ResourceManager.h"

int CreateDessertID(int baseid, int* type);

class ProducingProduct
{
    public:
    ProducingProduct() = default;
    ~ProducingProduct() = default;

    void Init(MaterialManager materialManager, DessertManager dessertManager)
    {
        PlateSize = -1;
        BaseMaterialID = -1;
        for(int i = 0; i < 3; i++)
        {
            CreateMaterialID[i] = -1;
            DecorationID[i] = -1;
        }
        for(int i = 0; i < 10; i++)
        {
            Type[i] = 0;
        }
        CreateNumber = 3;
        DecorateNumber = 3;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        color = {10,10,10,255};
        manager = materialManager;
        dManager = dessertManager;
        hasCalculated = 1;
        DelayTime = 0;
        WhetherRenderCreate = 1;
    }

    int GetDelayTime()
    {
        return DelayTime;
    }

    void SetPlateSize(int size)
    {
        PlateSize = size;
    }

    int GetPlateSize()
    {
        return PlateSize;
    }

    void SetBaseID(int id)
    {
        BaseMaterialID = id;
    }

    int GetBaseID()
    {
        return BaseMaterialID;
    }

    int GetLastCreateNumbers()
    {
        return 3 - CreateNumber;
    }

    int GetLastDecorationNumbers()
    {
        return 3 - DecorateNumber;
    }

    void RenderCreateNumbers(SDL_Renderer* Renderer)
    {
        std::string Cnumber = "剩余" + std::to_string(CreateNumber) + "次";
        CNumberSurface = TTF_RenderUTF8_Blended(TextFont,Cnumber.c_str(),color);
        int mw = CNumberSurface->w; int mh = CNumberSurface->h;
        CRect = {10,10,mw,mh};
        CNumberTexture = SDL_CreateTextureFromSurface(Renderer,CNumberSurface);
        SDL_RenderCopy(Renderer, CNumberTexture, nullptr, &CRect);
    }

    void RenderDecorationNumbers(SDL_Renderer* Renderer)
    {
        std::string Cnumber = "剩余" + std::to_string(DecorateNumber) + "次";
        CNumberSurface = TTF_RenderUTF8_Blended(TextFont,Cnumber.c_str(),color);
        int mw = CNumberSurface->w; int mh = CNumberSurface->h;
        CRect = {10,10,mw,mh};
        DNumberTexture = SDL_CreateTextureFromSurface(Renderer,CNumberSurface);
        SDL_RenderCopy(Renderer, DNumberTexture, nullptr, &CRect);
    }

    std::vector<int> GetDecorationID()
    {
        std::vector<int> a;
        for(int i = 0; i < 3; i++)
        {
            a.push_back(DecorationID[i]);
        }
        return a;
    }

    void moveUpdate(int CurrentTime)
    {
        for(int i = 0 ; i < 3; i++)
        {
            if(CreateMaterialID[i] != -1)
            {
                create[i].moveUpdate(CurrentTime,i);
            }
        }
    }

    void DmoveUpdate(int CurrentTime)
    {
        for(int i = 0 ; i < 3; i++)
        {
            if(DecorationID[i] != -1)
            {
                decorate[i].moveUpdate(CurrentTime,i);
            }
        }
    }

    void RenderCreateMaterial(SDL_Renderer* Renderer)
    {
        if(WhetherRenderCreate)
        {
            for(int i = 0; i < 3; i++)
            {
                if(CreateMaterialID[i] != -1)
                {
                    create[i].onRender(Renderer);
                }
            }
        }
    }
    void RenderDecorationMaterial(SDL_Renderer* Renderer)
    {
            for(int i = 0; i < 3; i++)
            {
                if(DecorationID[i] != -1)
                {
                    decorate[i].onRender(Renderer);
                }
            }
    }

    void SetCreateCase(int i, int j,MaterialManager mManager)
    {
        CreateMaterialID[i] = j;
        CreateMaterialType[i] = mManager.GetDecorationType(j);
        create[i] = manager.DecorationMaterial[j];
        CreateNumber--;
    }

    void ClearCreate()
    {
        for(int i = 0; i < 3; i++)
        {
            CreateMaterialType[i] = -1;
        }
    }

    void ClearDecoration()
    {
        for(int i = 0; i < 3; i++)
        {
            DecorationID[i] = -1;
        }
    }

    void SetDEcorationCase(int i, int j)
    {
        DecorationID[i] = j;
        decorate[i] = manager.DecorationMaterial[j];
        DecorateNumber--;
    }

    void SetDelayTime(int CurrentTime)
    {
        DelayTime = CurrentTime;
    }

    void Calculate(int CurrentTime)
    {
        if(hasCalculated)
        {
            for(int i = 0; i < 3; i++)
            {
                Type[CreateMaterialType[i]]++;
            }
            if(CurrentTime - DelayTime >= 1000)
            {
                BaseMaterialID = CreateDessertID(BaseMaterialID,Type);
                hasCalculated = 0;
                WhetherRenderCreate = 0;
            }
            for(int i = 0; i < 10; i++)
            {
                Type[i] = 0;
            }
        }
    }

    private:
    int PlateSize;
    int BaseMaterialID;
    int CreateMaterialID[3];
    int CreateMaterialType[3];
    int DecorationID[3];
    int CreateNumber;
    int DecorateNumber;
    TTF_Font* TextFont;
    SDL_Surface* CNumberSurface = nullptr;
    SDL_Surface* DNumberSurface = nullptr;
    SDL_Rect CRect;
    SDL_Rect DRect;
    SDL_Texture* CNumberTexture = nullptr;
    SDL_Texture* DNumberTexture = nullptr;
    SDL_Color color;
    MaterialManager manager;
    DessertManager dManager;
    Material create[3];
    Material decorate[3];
    int Type[10];
    int DelayTime;
    bool hasCalculated;
    bool WhetherRenderCreate;
};

int CreateDessertID(int baseid, int* type)
{
    switch(baseid)
    {
        case 0:
        {
            if(type[0] == 2 && type[1] == 1)
                return 1;
            if(type[0] == 1 && type[2] == 2)
                return 2;
            if(type[0] == 1 && type[4] == 1 && type[5] == 1)
                return 7;
            if(type[2] == 1 && type[3] == 1 && type[6] == 1)
                return 8;
            break;
        }
        case 3:
        {
            if(type[0] == 3)
                return 4;
            if(type[0] == 1 && type[1] == 2)
                return 5;
            if(type[0] == 1 && type[2] == 2)
                return 6;
        }
        default:break;
    }
    return baseid;
}
/*材料Type: | 蛋糕Type:
--0--奶油   | ----0----戚风----1----草莓
--1--草莓   | ----2--巧克力----3----果冻
--2--巧克力 | -4-奶油冰激凌--5-草莓冰激凌
--3--樱桃   | -6-巧克力冰激凌-7-提拉米苏
--4--咖啡   | ----8--黑森林
--5--饼干   |
--6--果汁   |
*/

class ProducedProduct
{
    public:
    ProducedProduct() = default;
    ~ProducedProduct() = default;

    void LoadInit(int id, int bid, std::vector<int> did, int size, MaterialManager mManager, DessertManager dManager)
    {
        ProductID = id;
        BaseDessertID = bid;
        for(int i = 0; i < 3; i++)
        {
            DecorationID.push_back(did[i]);
        }
        PlateSize = size;
        x = 200; y = 200;
        w = 200; h = 200;
        
        sweetlevel = dManager.Desserts[BaseDessertID].GetSweetNumber();
        fulllevel = dManager.Desserts[BaseDessertID].GetFullNumber();
        tastelevel = dManager.Desserts[BaseDessertID].GetTasteNumber();
        for(int i = 0; i < 3; i++)
        {
            if(DecorationID[i] >= 0)
            {
                //SDL_Log("当前：%d",DecorationID[i]);
                sweetlevel = sweetlevel + mManager.DecorationMaterial[DecorationID[i]].GetSweetNumber();
                fulllevel = fulllevel + mManager.DecorationMaterial[DecorationID[i]].GetFullNumber();
                tastelevel = tastelevel + mManager.DecorationMaterial[DecorationID[i]].GetTasteNumber();
            }    
        }
        RoundTexture = ResourceManager::instance()->FindTexture("round");
    }

    void SetRect(int mx,int my, int mw, int mh)
    {
        x = mx;
        y = my;
        w = mw;
        h = mh;
        Rect = {x,y,w,h};
    }

    void onRender(SDL_Renderer* Renderer,
        DessertManager dManager,
        MaterialManager mManager,
        std::vector<Plate>plates,
        int mx, int my, int mw, int mh)
    {
        Rect = {mx, my, mw, mh};
        RoundRect = {mx + 30, my + 30, mw -60, mh-60};
        SDL_RenderCopy(Renderer, RoundTexture, nullptr, &RoundRect);
        plates[PlateSize].onRender(Renderer, Rect);
        Rect.y = Rect.y - 25;
        dManager.onRender(Renderer,BaseDessertID, Rect);
        Rect.y = Rect.y - 30;
        for(int i = 0; i < 3; i++)
        {
            if(DecorationID[i] != -1)
            mManager.DecorationMaterial[DecorationID[i]].onRender(Renderer,Rect,i);
            // SDL_Log("%d",DecorationID[i]);
        }
    }

    int GetProductPrice()
    {
        // SDL_Log("当前三围如下：%d %d %d",sweetlevel,fulllevel,tastelevel);
        price = sweetlevel/10*3 + fulllevel/10 + tastelevel/10*2;
        return price;
    }

    std::string GetProductName(DessertManager dManager)
    {
        return dManager.GetDessertName(BaseDessertID);
    }

    int GetProductID(){return ProductID;}
    int GetDessertID(){return BaseDessertID;}
    std::vector<int> GetDecorationID(){return DecorationID;}
    int GetPlateSize(){return PlateSize;}

    private:
    int ProductID;
    int BaseDessertID;
    std::vector<int> DecorationID;
    int sweetlevel;
    int fulllevel;
    int tastelevel;
    int price;
    int PlateSize;
    SDL_Rect Rect;
    SDL_Rect RoundRect;
    int x,y;
    int w,h;
    SDL_Texture* BaseTexture;
    SDL_Texture* RoundTexture;
};

class Cloth
{
    public:
    Cloth() = default;
    ~Cloth() = default;

    void init()
    {
        ClothTexture = ResourceManager::instance()->FindTexture("cloth");
        LastTime = 0;
        DelayTime = 0;
        x = 0; y = -600;
        Rect = {x,y,600,600};
        a = 0;
    }

    void SetWhetherRender(bool a)
    {
        whetherRender = 1;
    }

    void SetWhetherAppear(bool b)
    {
        a = b;
    }

    void Appear(int CurrentTime)
    {
        if(a)
        {
            if(whetherRender == 1)
            {
                if(CurrentTime - LastTime >= 16 && y <= -50)
                {
                    y = y + 10;
                    Rect = {x,y,500,500};
                    LastTime = CurrentTime;
                }
            }
            if(y > -50)
            {
                DelayTime = CurrentTime;
                a = 0;
            }
        }
        
    }

    void Quit(int CurrentTime)
    {
        if(DelayTime != 0)
        {
            if(CurrentTime - DelayTime > 1000)
            {
                if(CurrentTime - LastTime >= 16 && y >= -600)
                {
                    y = y - 10;
                    Rect = {x,y,500,500};
                }
            }
            if(y < -600)
            {
                whetherRender = 0;
            }
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        if(whetherRender)
            SDL_RenderCopy(Renderer,ClothTexture, nullptr, &Rect);
    }

    private:
    SDL_Texture* ClothTexture;
    SDL_Rect Rect;
    int x,y;
    bool whetherRender;
    int LastTime;
    int DelayTime;
    int a;
};