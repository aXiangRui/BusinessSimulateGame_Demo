#pragma once

#include"RUI_Scene.h"
#include"RUI_SceneManager.h"
#include"RUI_MenuButton.h"
#include"RUI_Button.h"
#include"RUI_BaseMaterial.h"
#include"RUI_ResourceManager.h"
#include"RUI_MusicManager.h"
#include"RUI_ChooseFrame.h"
#include"RUI_Product.h"
#include"RUI_MaterialManager.h"
#include"RUI_ProductManager.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<vector>

class CreateRUIEvent
{
    public:
    CreateRUIEvent() = default;
    ~CreateRUIEvent() = default;

    std::vector<MenuButton> buttons;
    std::vector<RUI_Icon> Icons;
    std::vector<ChooseFrame> SizeFrames;
    std::vector<ChooseFrame> BaseFrames;
    std::vector<Dessert> BaseDessert;
    std::vector<Dessert> AllBaseDessert;
    std::vector<ChooseFrame> DecorationFrames;
    std::vector<Material> DecorationMaterials;
    std::vector<Plate> plates;
    ProducingProduct PProduct;
    ProductManager productManager;
    MaterialManager materialManager;  
    Cloth RedCloth; 

    void load()
    {
        CStage = Stage::size;
        TextFont = TTF_OpenFont("./resources/font/namidiansong.ttf",36);
        ChooseFrameTexture = ResourceManager::instance()->FindTexture("chooseframe");     
        dessertManager.InitDessertManager();
        materialManager.InitMaterialManager();
        productManager.InitProductManager();

        for(int i = 0; i < 3; i++)
        {
            Plate a;
            a.InitPlate(140,-60,i);
            plates.push_back(a);
        }

        for(int i = 0; i < 100; i++)
        {
            BaseID[i] = -1;
        }
        
        RUI_Icon exiticon;
        exiticon.InitIcon(10,500,50,50,0,"exiticon");
        Icons.push_back(exiticon);

        RUI_Icon nexticon;
        nexticon.InitIcon(740, 540, 50, 50, 1, "nexticon");
        Icons.push_back(nexticon);

        RUI_Icon lasticon;
        lasticon.InitIcon(680, 540, 50, 50, 2, "nexticon");
        Icons.push_back(lasticon);

        MenuButton Btn0((WindowWidth-320)/2 + 200,500,320,64,"完成",0);
        buttons.push_back(Btn0);
        PProduct.Init(materialManager,dessertManager);
        RedCloth.init();
        WhetherBack = 0;
        ChoosePage = 0;
    }

    void ChooseSize(std::vector<int>& Size)
    {
        DessertSize = Size;
        for(int i = 0; i < DessertSize.size(); i++)
        {
            ChooseFrame AddFrame;
            std::string a;
            switch(i)
            {
                case 0: a = "小";break;
                case 1: a = "中";break;
                case 2: a = "大";break;
                default:break;
            }
            AddFrame.InitChooseFrame(500, 10 + i * 64, i, a.c_str());
            SizeFrames.push_back(AddFrame);
        }
    }

    void ChooseBase()
    {
        int j = 0;
        for(int i = 0; i < dessertManager.GetDessertsSize(); i++)
        {         
            if(dessertManager.GetWhetherBase(i))
            { 
                ChooseFrame AddFrames;
                std::string name = dessertManager.GetDessertName(i);
                AddFrames.InitChooseFrame(500, 10 + j * 64 - (j / 8) * 64 * 8 , j, name.c_str());
                BaseFrames.push_back(AddFrames);
                AllBaseDessert.push_back(dessertManager.Desserts[i]);
                // ensure the newly-pushed dessert has its position initialized
                AllBaseDessert.back().SetPosition(140, -100);
                AllBaseDessert.back().SetWidth(256,256);
                BaseID[j] = i;
                // SDL_Log("基类蛋糕:%s,%d,%d",BaseDessert.back().GetDessertName().c_str(),BaseDessert.back().GetDessertID(),i);
                j++;
            }
            BaseDessert.push_back(dessertManager.Desserts[i]);
            BaseDessert.back().SetPosition(140, -100);
            BaseDessert.back().SetWidth(256,256);
        }
    }

    void ChooseDecoration()
    {
        for(int i = 0; i < materialManager.GetDecorationSize(); i++)
        {
            ChooseFrame AddFrames;
            std::string name = materialManager.GetDecorationName(i);
            AddFrames.InitChooseFrame(500,10 + i * 64 - (i / 8) * 64 * 8, i, name);
            DecorationFrames.push_back(AddFrames);
            DecorationMaterials.push_back(materialManager.DecorationMaterial[i]);
        }
    }

    void update()
    {
        CurrentTime = SDL_GetTicks();
        if(PProduct.GetPlateSize() != -1)
        {
            plates[PProduct.GetPlateSize()].MovitionUpdate(CurrentTime,5);
        }
        if(PProduct.GetBaseID() != -1)
        {
            for(int i = 0; i < BaseDessert.size(); i++)
            {
                BaseDessert[i].moveUpdate(CurrentTime);
            }
            // BaseDessert[PProduct.GetBaseID()].moveUpdate(CurrentTime);
        }
        PProduct.moveUpdate(CurrentTime);
        PProduct.DmoveUpdate(CurrentTime);
        RedCloth.Appear(CurrentTime);
        RedCloth.Quit(CurrentTime);   
        if(PProduct.GetDelayTime() != 0)
        {
            PProduct.Calculate(CurrentTime);
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {    
        if(PProduct.GetPlateSize() != -1)
            plates[PProduct.GetPlateSize()].onRender(Renderer);
        if(PProduct.GetBaseID() != -1)
        {
            // if(PProduct.GetBaseID() < BaseDessert.size())
            // SDL_Log("当前甜点id,%d",PProduct.GetBaseID());
            BaseDessert[PProduct.GetBaseID()].onRender(Renderer);
            // SDL_Log("当前甜点名字%s",dessertManager.GetDessertName(PProduct.GetBaseID()).c_str());
            // dessertManager.onRender(Renderer, PProduct.GetBaseID());
        }
        PProduct.RenderCreateMaterial(Renderer);
        PProduct.RenderDecorationMaterial(Renderer);

    switch(CStage)
    {
        case Stage::size:
        {
            for(int i = 0; i < SizeFrames.size(); i++)
            {
                if(i / 8 == ChoosePage)
                {
                    if(SizeFrames[i].GetIsHovered())
                    {
                        SizeFrames[i].onHoverRender(Renderer);
                        plates[i].AnimationRender(Renderer,CurrentTime);
                    }
                    else
                    {
                        SizeFrames[i].onRender(Renderer);
                    }       
                }       
            }
            break;
        }
        case Stage::base:
        {
            for(int i = 0; i < BaseFrames.size(); i++)
            {
                if(i / 8 == ChoosePage)
                {
                    if(BaseFrames[i].GetIsHovered())
                    {
                        BaseFrames[i].onHoverRender(Renderer);
                        dessertManager.onRender(Renderer, BaseID[i]);
                        BaseDessert[BaseID[i]].onRender(Renderer);
                    }
                    else
                    {
                        BaseFrames[i].onRender(Renderer);
                    }
                } 
            }
            break;
        }
        case Stage::create:
        {
            PProduct.RenderCreateNumbers(Renderer);
            for(int i = 0; i < DecorationFrames.size(); i++)
            {
                if( i / 8 == ChoosePage )
                {
                    if(DecorationFrames[i].GetIsHovered())
                    {
                        DecorationFrames[i].onHoverRender(Renderer);
                        DecorationMaterials[i].onRender(Renderer);
                    }
                    else
                    {
                        DecorationFrames[i].onRender(Renderer);
                    }
                }            
            }
            break;
        }
        case Stage::decorate:
        {
            PProduct.RenderDecorationNumbers(Renderer);
            for(int i = 0; i < DecorationFrames.size(); i++)
            {
                if(i / 8 == ChoosePage)
                {
                    if(DecorationFrames[i].GetIsHovered())
                    {
                        DecorationFrames[i].onHoverRender(Renderer);
                        DecorationMaterials[i].onRender(Renderer);
                    }
                    else
                    {
                        DecorationFrames[i].onRender(Renderer);
                    }
                }
            }
            for(int i = 0; i < buttons.size(); i++)
            {
                buttons[i].ButtonRender(Renderer);
            }
            break;    
        }
        default:break;
    }
    for(int i = 0; i < Icons.size(); i++)
        {
            if(i != 2)
                Icons[i].onRender(Renderer);
            else
                {Icons[i].onRender(Renderer,1);} 
        }
    RedCloth.onRender(Renderer);
    }

    void input(const SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_MOUSEMOTION:
            {
                int mx = event.motion.x; int my = event.motion.y;
                int j = 0;
                switch (CStage)
                {
                    case Stage::size:
                    {
                        for(int i = 0; i < SizeFrames.size(); i++)
                        {
                            if(SizeFrames[i].isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }                        
                        }
                        // if(!j)
                        //     SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                        break;
                    }
                    case Stage::base:
                    {
                        for(int i = 0; i < BaseFrames.size(); i++)
                        {
                            if( i / 8 == ChoosePage)
                            {
                                if(BaseFrames[i].isHovered(mx,my))
                                {
                                    j = 1;
                                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                                }
                            }
                        }
                        // if(!j)
                        //     SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                        break;
                    }
                    case Stage::create:
                    {
                        for(int i = 0; i < DecorationFrames.size(); i++)
                        {
                            if(DecorationFrames[i].isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }
                        }
                    }
                    case Stage::decorate:
                    {
                        for(int i = 0; i < DecorationFrames.size(); i++)
                        {
                            if(DecorationFrames[i].isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }
                        }
                        for(int i = 0; i < buttons.size(); i++)
                        {
                            if(buttons[i].RUI_isHovered(mx,my))
                            {
                                j = 1;
                                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                            }
                        }
                        break;
                    }
                    default:break;
                }
                for(int i = 0; i < Icons.size(); i++)
                {
                    if(Icons[i].isHovered(mx,my))
                    {
                        j = 1;
                        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                    }
                }
                if(!j)
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                int mx = event.button.x; int my = event.button.y;
                for(int i = 0; i <= Icons.size(); i++)
                {
                    if(Icons[i].isClicked(mx,my))
                    {
                        switch(i)
                        {
                            case 0:
                            {
                                SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game);
                                break;
                            }
                            case 1:
                            {
                                ChoosePage = ChoosePage + 1;
                                break;
                            }
                            case 2:
                            {
                                ChoosePage = ChoosePage - 1;
                                if(ChoosePage < 0)
                                ChoosePage = 0;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                switch(CStage)
                {
                    case Stage::size:
                    {
                        for(int i = 0; i < SizeFrames.size(); i++)
                        {
                            if(SizeFrames[i].GetIsHovered())
                            {     
                                //SDL_Log("当前按下的坐标:%d,%d,%d",mx,my,i);
                                plates[i].SetMoveCheck(1);
                                PProduct.SetPlateSize(i);
                                ChoosePage = 0;
                                CStage = Stage::base;
                            }
                        }
                        break;        
                    }
                    case Stage::base:
                    {
                        for(int i = 0; i < BaseFrames.size(); i++)
                        {
                            if(i / 8 == ChoosePage)
                            {
                                if(BaseFrames[i].GetIsHovered())
                                {
                                    BaseDessert[i].SetIsClicked(1);
                                    // PProduct.SetBaseID(BaseDessert[i].GetDessertID());
                                    PProduct.SetBaseID(BaseID[i]);
                                    ChoosePage = 0;
                                    CStage = Stage::create;
                                }
                            }
                        }
                        break;
                    }
                    case Stage::create:
                    {
                        int n = PProduct.GetLastCreateNumbers();
                        for(int i = 0; i < DecorationFrames.size(); i++)
                        {
                            if(i / 8 == ChoosePage)
                            {
                                if(DecorationFrames[i].isHovered(mx,my))
                                {
                                    if(n <= 2)
                                    {
                                        PProduct.SetCreateCase(n,DecorationMaterials[i].GetID(), materialManager);
                                    }
                                        
                                    if(n == 2)
                                    {
                                        ChoosePage = 0;
                                        CStage = Stage::decorate;
                                        RedCloth.SetWhetherAppear(1);
                                        RedCloth.SetWhetherRender(1);
                                        PProduct.SetDelayTime(CurrentTime);
                                    } 
                                }
                            }
                        }
                        break;
                    }
                    case Stage::decorate:
                    {
                        for(int i = 0; i < DecorationFrames.size(); i++)
                        {
                            if(i / 8 == ChoosePage)
                            {
                                int n = PProduct.GetLastDecorationNumbers();
                                if(DecorationFrames[i].isHovered(mx,my))
                                {
                                    if(n <= 2)
                                    {
                                        PProduct.SetDEcorationCase(n,DecorationMaterials[i].GetID());
                                    }
                                    if(n == 2)
                                    {
                                        SDL_Log("制作完成");
                                    }
                                }
                            }
                        }
                        for(int i = 0; i < buttons.size(); i++)
                        {
                            if(buttons[i].RUI_isClicked(mx,my))
                            {
                                switch(i)
                                {
                                    case 0:
                                    {
                                        ProducedProduct a;
                                        a.LoadInit
                                        (
                                        productManager.GetProductSize(),
                                        PProduct.GetBaseID(),
                                        PProduct.GetDecorationID(),
                                        PProduct.GetPlateSize(),
                                        materialManager,
                                        dessertManager
                                        );
                                    productManager.AddProduct(a);
                                    productManager.Save();
                                    SDL_Log("添加新产品:%d",productManager.GetProductSize());
                                    WhetherBack = 1;
                                    break;
                                    }
                                }
                            }
                        }
                    }
                    default:break;
                }
                break;
            }
        }
    }

    void quit()
    {
        Icons.clear();
        plates.clear();
        DessertSize.clear();
        SizeFrames.clear();
        BaseFrames.clear();
        BaseDessert.clear();
        dessertManager.quit();
        materialManager.quit();
        DecorationFrames.clear();
        DecorationMaterials.clear();  
        productManager.quit();
        RedCloth.SetWhetherRender(0);
    }

    void SetStage(int i)
    {
        switch(i)
        {
            case 1:{CStage = Stage::size;     break;}
            case 2:{CStage = Stage::base;     break;}
            case 3:{CStage = Stage::create;   break;}
            case 4:{CStage = Stage::decorate; break;}
            default:break;
        }
    }

    bool GetWhetherBack(){return WhetherBack;}

    enum class Stage
    {
        size,
        base,
        create,
        decorate
    };

    Stage CStage;
    std::vector<int> DessertSize;
    TTF_Font* TextFont;
    SDL_Texture* ChooseFrameTexture;
    int CurrentTime;
    DessertManager dessertManager;
    bool WhetherBack;
    int ChoosePage;
    int BaseID[100];
};