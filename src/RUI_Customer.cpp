#include "../include/RUI_Customer.h"

void Customer::InitCustomer(int id, int preferid, std::string name, std::string path, int appear, int prefer)
{
    CustomerID = id;
    PreferDessertID = preferid;
    CustomerName = name;
    PathName = path;
    x = 800;
    y = 450;
    SitTime = 0;
    ChooseTime = 0;
    WhetherAppear = appear;
    preference = prefer;

    CurrentStage = CustomerStage::Enter;
    EnterFinish = 0;
    ChooseFinish = 0;
    BuyFinish = 0;
    EatFinsih = 0;
    QuitFinish = 0;
    toward = 0;
    isEating = -1;
    isGoingPay = 0;
    PayTime = 0;
    isFront = 0;
    Queue = 0;
    ChooseNumber = 0;
    payPrice = 0;
    hasJoined = 0;
    isHovered = 0;
    RandomDelay = rand() % 10000;
    waitingTime = 0;
    onSeat = 0;
    addNumber = 0;
    whetherRenderAdd = 0;
    EatTime = 0;

    NameFont = TTF_OpenFont("./resources/font/namidiansong.ttf",16);
    color = {10,10,10,255};
    std::string totalname = CustomerName + " " + std::to_string(preference);
    NameSurface = TTF_RenderUTF8_Blended(NameFont, totalname.c_str(), color);
    AddFrameSurface = nullptr;
    if(!NameSurface)
        SDL_Log("%s",CustomerName.c_str());
    NameW = NameSurface->w; NameH = NameSurface->h;

    payCharm.Init();
    CakeTexture = ResourceManager::instance()->FindTexture("smallcake");
    PlateTexture = ResourceManager::instance()->FindTexture("tinyplate");
}

void Customer::RenderCake(SDL_Renderer* Renderer)
{  
    if(toward == 0)
    {
        PlateRect = { x - 16, y - 8, 64, 64};
    }
    else
    {
        PlateRect = {x + 16, y - 8, 64, 64};
    }
    if(onSeat)
    {
        if( toward == 0)
        {
            PlateRect = { x - 24, y - 12, 64, 64};
        }
        else
        {
            PlateRect = { x + 24, y - 12, 64, 64};
        }
    }
    if(toward == 0)
    {
        SDL_RenderCopy(Renderer, PlateTexture, nullptr, &PlateRect);
    }
    else
    {
        SDL_RenderCopyEx( Renderer, PlateTexture, nullptr, &PlateRect, 0, 0, SDL_FLIP_HORIZONTAL);
    }
    
    for( int i = 0; i < eatNumber; i++)
    {
        if(toward == 0)
        {
            CakeRect = {x - 16, y - i * 8 - 12, 64, 64};
        }    
        else
        {
            CakeRect = {x + 16, y - i * 8 - 12, 64, 64};
            PlateRect = { x + 16, y, 64, 64};
        }
        if(CurrentStage == CustomerStage::Eat && onSeat == 1)
        {
            if(toward == 0)
            {
                CakeRect = {x - 24, y - i * 8 - 14, 64, 64};
            }
            
            else
            {
                CakeRect = {x + 24, y - i * 8 - 14, 64, 64};    
            }
        }
        if(toward == 0)
        {
            SDL_RenderCopy(Renderer, CakeTexture, nullptr, &CakeRect);
        }
        else
        {
            SDL_RenderCopyEx( Renderer, CakeTexture, nullptr, &CakeRect, 0, 0, SDL_FLIP_HORIZONTAL);
        }        
    }
}

void Customer::SetEatNumber( int number )
{
    eatNumber = number;
}

int Customer::GetEatNumber()
{
    return eatNumber;
}

void Customer::SetChooseNumber()
{
    if( preference <= Level[1])
        ChooseNumber = 1 ;
    else if( preference <= Level[3])
        ChooseNumber = rand() % 2 + 2;
    else if( preference <= Level[5])
        ChooseNumber = rand() % 3 + 3;
    else if( preference <= Level[7])
        ChooseNumber = rand() % 4 + 4;
    else
    {
        ChooseNumber = rand() % 5 + 5;
        if(ChooseNumber < 6)
            ChooseNumber++;
    }
    eatNumber = ChooseNumber;
    SDL_Log("%s当前好感度:%d,选择数量%d",CustomerName.c_str(),preference,ChooseNumber);
}

void Customer::RenderAddFrame(SDL_Renderer* Renderer)
{
    std::string add = "+" + std::to_string(addNumber);
    if(AddFrameSurface == nullptr)
    {
        AddFrameSurface = TTF_RenderUTF8_Blended(NameFont, add.c_str(), color);
        AddFrameRect = {x-10, y, AddFrameSurface->w,AddFrameSurface->h};
    }
    AddFrameTexture = SDL_CreateTextureFromSurface(Renderer, AddFrameSurface);
    SDL_RenderCopy(Renderer, AddFrameTexture, nullptr, &AddFrameRect );
}

int Customer::GetAddNumber(int DessertID,int chooseNumber)
{
    if(PreferDessertID == -1)
            {
                addNumber = 6 * chooseNumber;
            }
            else if(DessertID == PreferDessertID)
            {
                addNumber = 6 * chooseNumber;
                // SDL_Log("01增加了%d",6*chooseNumber);
            }
            else
            {
                addNumber = chooseNumber;
                //  SDL_Log("02增加了%d",chooseNumber);
            }
            return addNumber;
}