#pragma once

#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include<fstream>
#include"RUI_Dessert.h"
#include"RUI_DessertManager.h"
#include"RUI_ResourceManager.h"
#include"RUI_Chair.h"
#include"RUI_Cabinet.h"
#include"RUI_Clock.h"
#include"RUI_ProductManager.h"

enum class CustomerStage
{
    Enter,
    Choose,
    Buy,
    Eat,
    Leave
};

class Customer
{
    public:
        Customer() = default;
        ~Customer() = default;
        std::vector<SDL_Texture*>CustomerTexture;
        SDL_Texture* NormalTexture = nullptr;
        SDL_Texture* NameTexture = nullptr;
        SDL_Surface* NameSurface;
        SDL_Rect NameRect;
        TTF_Font* NameFont = nullptr;
        SDL_Color color;
        int NameW,NameH;

        void InitCustomer(int id, int preferid, std::string name, std::string path, int appear, int prefer)
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

            NameFont = TTF_OpenFont("./resources/font/namidiansong.ttf",16);
            color = {10,10,10,255};
            std::string totalname = CustomerName + " " + std::to_string(preference);
            NameSurface = TTF_RenderUTF8_Blended(NameFont, totalname.c_str(), color);
            if(!NameSurface)
                SDL_Log("%s",CustomerName.c_str());
            NameW = NameSurface->w; NameH = NameSurface->h;

            payCharm.Init();
        }

        void SetCurrentStage(int i)
        {
            switch(i)
            {
                case 0:
                    CurrentStage = CustomerStage::Enter;
                    break;
                case 1:
                    CurrentStage = CustomerStage::Choose;
                    break;
                case 2:
                    CurrentStage = CustomerStage::Buy;
                    break;
                case 3:
                    CurrentStage = CustomerStage::Eat;
                    break;
                case 4:
                    CurrentStage = CustomerStage::Leave;
                    break;
                default:break;
            }
        }

        int GetCurrentStage()
        {
            switch (CurrentStage)
            {
            case CustomerStage::Enter:
                return 0;
                break;
            case CustomerStage::Choose:
                return 1;
                break;
            case CustomerStage::Buy:
                return 2;
                break;
            case CustomerStage::Eat:
                return 3;
                break;
            case CustomerStage::Leave:
                return 4;
                break;
            default:
                break;
            }
            return -1;
        }

        int GetX()
        {
            return x;
        }

        int GetY()
        {
            return y;
        }

        void SetX(int mx)
        {
            x = mx;
        }

        void SetY(int my)
        {
            y = my;
        }

        void SetChooseNumber()
        {
            if( preference <= Level[1])
                ChooseNumber = 1;
            else if( preference <= Level[3])
                ChooseNumber = rand() % 2 + 1;
            else if( preference <= Level[5])
                ChooseNumber = rand() % 3 + 1;
            else if( preference <= Level[7])
                ChooseNumber = rand() % 4 + 1;
            else
            {
                ChooseNumber = rand() % 5 + 1;
                if(ChooseNumber < 2)
                    ChooseNumber++;
            }    
            SDL_Log("%s当前好感度:%d,选择数量%d",CustomerName.c_str(),preference,ChooseNumber);
        }

        int GetCustomerID()
        {
            return CustomerID;
        }
        
        int GetPreferDessertID()
        {
            return PreferDessertID;
        }

        std::string GetCustomerName()
        {
            return CustomerName;
        }

        std::string GetCustomerPath()
        {
            return PathName;
        }

        int GetCustomerPreference()
        {
            return preference;
        }

        void SetChooseNumber(int Number)
        {
            ChooseNumber = Number;
        }

        int GetChooseNumber()
        {
            return ChooseNumber;
        }

        void SetSitTime(int time)
        {
            SitTime = time;
        }

        void SetChooseTime(int time)
        {
            ChooseTime = time;
        }

        int GetSitTime()
        {
            return SitTime;
        }

        int GetChooseTime()
        {
            return ChooseTime;
        }

        void loadTexture(std::string path)
        {
            CustomerTexture.push_back(ResourceManager::instance()->FindTexture(path));
        }

        void OnRender(SDL_Renderer* Renderer)
        {
            if(!Renderer)
            {
                SDL_Log("OnRender: Renderer is null for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            NormalTexture = ResourceManager::instance()->FindTexture(PathName.c_str());
            if(!NormalTexture)
            {
                SDL_Log("OnRender: texture not found for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            SDL_Rect Rect = {x,y,64,64};
            if(toward == 0)
            {
                SDL_RenderCopy(Renderer,NormalTexture,nullptr,&Rect);
            }
            else
            {
                SDL_RenderCopyEx(Renderer,NormalTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
            }

            if(payCharm.GetMoney() != 0)
                payCharm.onRender(Renderer);
        }

        void onRenderWithName(SDL_Renderer* Renderer)
        {      
            if(!Renderer)
            {
                SDL_Log("OnRender: Renderer is null for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            NormalTexture = ResourceManager::instance()->FindTexture(PathName.c_str());
            if(!NormalTexture)
            {
                SDL_Log("OnRender: texture not found for customer id=%d name=%s", CustomerID, CustomerName.c_str());
                return;
            }
            SDL_Rect Rect = {x,y,64,64};
            if(toward == 0)
            {
                SDL_RenderCopy(Renderer,NormalTexture,nullptr,&Rect);
            }
            else
            {
                SDL_RenderCopyEx(Renderer,NormalTexture,nullptr,&Rect,0,0,SDL_FLIP_HORIZONTAL);
            }

            // create a local texture for the name surface and destroy it immediately
            SDL_Texture* nameTex = SDL_CreateTextureFromSurface(Renderer, NameSurface);
            NameRect = {x+15,y,NameW,NameH};
            SDL_RenderCopy(Renderer, nameTex, nullptr, &NameRect);
            SDL_DestroyTexture(nameTex);

            if(payCharm.GetMoney() != 0)
                payCharm.onRender(Renderer);
        }

        void Update(std::vector<Chair>& Chairs,
            int currentTime, 
            std::vector<Cabinet>& Cabtines,
            DessertManager dessertManager,
            ProductManager pManager,
            Customer& customer,
            int&  TotalMoney
        )
        {
            switch(CurrentStage)
            {
                case CustomerStage::Enter:
                {
                    EnterStore(currentTime,Cabtines.size());
                    break;
                }
                case CustomerStage::Choose:
                {
                    ChooseDessert(Cabtines,currentTime,pManager);
                    break;
                }
                case CustomerStage::Buy:
                {
                    Pay(currentTime,TotalMoney,pManager,Cabtines);
                    break;
                }
                case CustomerStage::Eat:
                {
                    Eat(Chairs, currentTime, customer, Cabtines);
                    break;
                }
                case CustomerStage::Leave:
                {
                    LeaveStore();
                    break;
                }
            }
        }

        void EnterStore(int currentTime, int CabinetSize)
        {
            if(x >= 450)
            {
                toward = 0;
                x = x - speed;
            }
            else if(x < 450 && y <= 500 && x >=200 )
            {
                y = y + speed;
            }
            else if(x >= 200 && x < 450 && y > 500)
            {
                toward = 0;
                x = x - speed;
            }
            else if(x < 200 && y >= 450)
            {
                y = y - speed;
            }
            if(x < 200 && y < 450)
            {
                SetChooseID(rand()%CabinetSize);
                CurrentStage = CustomerStage::Choose;
            }
        }

        void ChooseDessert(std::vector<Cabinet>&Cabinets, int currentTime, ProductManager pManager)
        {
            if( ChooseTime == 0)
            {
                ChooseTime = currentTime;
            }
            if(x < Cabinets[chooseID].GetX() + 32)
            {
                toward = 1;
                x = x + speed;
            }
            else if(x > Cabinets[chooseID].GetX() + 32)
            {
                toward = 0;
                x = x - speed;
            }
            else if(y < Cabinets[chooseID].GetY() - 16)
            {
                toward = 0;
                y = y + speed;
            }
            else if(y > Cabinets[chooseID].GetY() - 16)
            {
                toward = 0;
                y = y - speed;
            }
            else if(x == Cabinets[chooseID].GetX() + 32 && y == Cabinets[chooseID].GetY() - 16)
            {
                toward = 0;
                if(currentTime - ChooseTime >= 5000 + rand() % 500 - 250)
                {
                    int dID = Cabinets[chooseID].GetDessertID();
                    SDL_Log("%s 选择了%d",CustomerName.c_str(), chooseID);
                    int price = pManager.GetProductPrice(dID);
                    SDL_Log("价格为%d",price);
                    payPrice = price * ChooseNumber;
                    CurrentStage = CustomerStage::Buy;
                    ChooseTime = 0;
                }
            }
        }

        void Pay(int CurrentTime, int& TotalMoney, ProductManager& pManager, std::vector<Cabinet>& Cabinets)
        {
            if(y >= 150 + Queue * 2)
            {
                y = y - speed;
            }
            else if(x <= maxNumber(350 - Queue * 20 , 0))
            {
                toward = 1;
                x = x + speed;
                if(x == 0)
                    toward = 1;
            }
            else if(x > maxNumber(350 - Queue * 20 + 10 , -10))
            {
                toward = 0;
                x = x - speed;
                if(x == 0)
                    toward = 1;
            }
            if(x > 350 && y < 150)
            {       
                if( PayTime == 0)
                {
                    PayTime = CurrentTime;
                    SDL_Log("%s此时的时间%d",CustomerName.c_str(),PayTime);
                }
                toward = 1;
                if(CurrentTime - PayTime >= 1000 && isGoingPay == 1)
                {
                    if(payPrice >= 1000)
                        SDL_Log("warnning!!!!价格超标，数据异常:%d",payPrice);
                    int dID = Cabinets[chooseID].GetDessertID();
                    int price = pManager.GetProductPrice(dID);
                    payPrice = price * ChooseNumber;
                    TotalMoney = TotalMoney + payPrice;

                    // SDL_Log("增加价钱%d,%d,%d",payPrice,price,ChooseNumber);
                    payCharm.SetPrice(payPrice);
                    payCharm.SetStartTime(CurrentTime);
                    payCharm.SetStopTime(CurrentTime);
                    // SDL_Log("已付款，当前总金额:%d",TotalMoney);
                    
                    SDL_Log("此时%s数据%d,%d",CustomerName.c_str(),ChooseNumber,chooseID);
                    CurrentStage = CustomerStage::Eat;
                    PayTime = 0;
                }
            }
        }

        void Eat(std::vector<Chair>& Chairs, int CurrentTime, Customer& customer, std::vector<Cabinet>& Cabinets)
        {
            if(SitTime == 0)
            {
                SitTime = CurrentTime;
            }
            if(isEating == -1)
            {
                for(int i = 0; i < Chairs.size(); i++)
                {
                    if(isEating != -1)
                        break;
                    else
                    {
                        if(Chairs[i].GetUsing() == 0)
                        {
                            Chairs[i].SetUsing(1);
                            isEating = i;
                        }
                    } 
                }             
                if(isEating == -1)
                {
                    if(x <= 600)
                        x = x + speed;
                    // CurrentStage = CustomerStage::Leave;
                }
            }
            else
            {
                if( x != Chairs[isEating].GetX())
                {
                    if(x - Chairs[isEating].GetX() >= 0)
                    {
                        x = x - speed;
                        toward = 0;
                    }
                    else
                    {
                        x = x + speed;
                        toward = 1;
                    }
                }
                else if( y != Chairs[isEating].GetY())
                {
                    if(y - Chairs[isEating].GetY() >= 0)
                    {
                        y = y - speed;
                    }
                    else
                    {
                        y = y + speed;
                    }        
                }
                if(x == Chairs[isEating].GetX() && y == Chairs[isEating].GetY())
                {
                    if(isEating % 2 == 0)
                    {
                        toward = 1;
                    }
                    else
                    {
                        toward = 0;
                    }
                    
                    if(CurrentTime - SitTime >= 18000 + rand()% 5000)
                    {    
                        Chairs[isEating].SetUsing(0);
                        // 更新 manager 中的模板顾客的喜好值
                        customer.AddPreference(Cabinets[chooseID].GetDessertID(),ChooseNumber);
                        // 将最新的喜好值同步回当前场景中的顾客实例
                        this->preference = customer.GetCustomerPreference();
                        // 更新名字的 Surface（用于 onRenderWithName）
                        RefreshNameSurface();
                        CurrentStage = CustomerStage::Leave;
                        SitTime = 0;
                    }
                }
            }
            payCharm.SetStopTime(CurrentTime);
        }

        // 刷新名字的 surface（在 preference 或名字变更后调用）
        void RefreshNameSurface()
        {
            if(!NameFont)
                NameFont = TTF_OpenFont("./resources/font/namidiansong.ttf",16);
            if(NameSurface)
            {
                SDL_FreeSurface(NameSurface);
                NameSurface = nullptr;
            }
            std::string totalname = CustomerName + " " + std::to_string(preference);
            NameSurface = TTF_RenderUTF8_Blended(NameFont, totalname.c_str(), color);
            if(!NameSurface)
                SDL_Log("RefreshNameSurface: failed for %s", CustomerName.c_str());
            else
            {
                NameW = NameSurface->w;
                NameH = NameSurface->h;
            }
        }

        void SetChooseID(int i)
        {
            chooseID = i;
        }

        int GetChooseID()
        {
            return chooseID;
        }

        void LeaveStore()
        {
            if(x <= 580 && y <= 350)
            {
                toward = 1;
                x = x + speed;
            }
            else if(x > 590 && y <= 350)
            {
                toward = 0;
                x = x - speed;
            }
            else if(y <= 350)
            {
                y = y + speed;
            }
            else if(x <= 800)
            {
                toward = 1;
                x = x + speed;
            }
            if(x > 800 && y > 350 && CurrentStage == CustomerStage::Leave)
            {
                QuitFinish = 1;
            }
        }

        bool GetQuit()
        {
            return QuitFinish;
        }

        bool GetToward()
        {
            return toward;
        }

        int getX()
        {
            return x;
        }

        int getY()
        {
            return y;
        }

        bool isGoingToPay(int CurrentTime)
        {
            if( CurrentStage == CustomerStage::Buy && isGoingPay == 0)
            {
                return 1;
            }
            return 0;
        }

        bool hasPayed()
        {
            if( CurrentStage == CustomerStage::Eat)
            {
                return 1;
            }
            return 0;
        }

        void setIsGoingToPay(bool x)
        {
            isGoingPay = x;
        }

        void setIsfront(bool x)
        {
            isFront = x;
        }

        int GetPayTime()
        {
            return PayTime;
        }

        void SetPayTime(int time)
        {
            PayTime = time;
        }

        void SetQueueNumber(int number)
        {
            Queue = number;
        }

        int maxNumber(int a,int b)
        {
            if(a > b)
                return a;
            return b;
        }

        bool WhetherAdd(int size, Clock clock)
        {
            if(hasJoined)
                return 0;
            else if(size >= (2 * clock.ReturnDay() + 10))
            {
                if(rand() % 8 >= 7)
                {
                    return true;
                }
                return false;
            }
            else
            {
                if(rand() % 2 == 1)
                {
                    return true;
                }
                return false;
            }
            
        }

        int GetDelayTime()
        {
            return RandomDelay;
        }

        void SetHasJoined(bool a)
        {
            hasJoined = a;
        }

        int GetHasJoined()
        {
            return hasJoined;
        }

        void SetIsHovered(bool i)
        {
            isHovered = i;
        }

        bool GetIsHoverd()
        {
            return isHovered;
        }

        void SetWhetherAppear(bool a)
        {
            WhetherAppear = a;
        }

        bool GetWhetherAppear()
        {
            return WhetherAppear;
        }

        void AddPreference(int DessertID,int chooseNumber)
        {
            if(PreferDessertID == -1)
            {
                preference = preference + 6 * chooseNumber;

            }
            else if(DessertID == PreferDessertID)
            {
                preference = preference + 6 * chooseNumber;
                // SDL_Log("01增加了%d",6*chooseNumber);
            }
            else
            {
                preference = preference + chooseNumber;
                //  SDL_Log("02增加了%d",chooseNumber);
            }
            // preference = preference + 5;
            if(preference > 2000)
                    preference = 2000;
            SDL_Log("当前喜好值%d",preference);
        }

        void Clean()
        {
            // NameTexture is created/destroyed each render call now, so don't destroy it here.
            if(NameSurface)
                SDL_FreeSurface(NameSurface);
            // NormalTexture is owned by ResourceManager (shared). Do NOT destroy it here.
            TTF_CloseFont(NameFont);
        }

    private:
        std::string CustomerName;
        std::string PathName;
        int CustomerID;
        int PreferDessertID;
        int x,y;
        int SitTime;
        int ChooseTime;
        CustomerStage CurrentStage;

        int chooseID;

        bool EnterFinish;
        bool ChooseFinish;
        bool BuyFinish;
        bool EatFinsih;
        bool QuitFinish;
        int isEating;
        bool toward;
        int preference;
        int speed = 2;
        int Queue;

        bool isGoingPay;
        bool isFront;
        int PayTime;
        int ChooseNumber;
        int payPrice;
        int RandomDelay;

        bool hasJoined;
        bool isHovered;
        bool WhetherAppear;
        int Level[10] = {5,20,50,100,200,350,500,700,1000,1500};
        PayCharm payCharm;
};
