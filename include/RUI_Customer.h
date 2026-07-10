#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <fstream>
#include "RUI_Dessert.h"
#include "RUI_DessertManager.h"
#include "RUI_ResourceManager.h"
#include "RUI_Chair.h"
#include "RUI_Cabinet.h"
#include "RUI_Clock.h"
#include "RUI_ProductManager.h"
#include "RUI_Tool.h"
#include "RUI_Pathfinding.h"

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

        void InitCustomer(int id, int preferid, std::string name, std::string path, int appear, int prefer);

        void SetCurrentStage(int i)
        {
            path.clear();
            pathIndex = 0;
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

        int GetSortedY(){return y + 64 + offsetY;}

        void SetChooseNumber();

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
            SDL_Rect Rect = {x,y+offsetY,64,64};
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
            if(whetherRenderAdd == 1)
            {
                RenderAddFrame(Renderer);
            }
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
            SDL_Rect Rect = {x,y+offsetY,64,64};
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
            NameRect = {x+15,y+offsetY,NameW,NameH};
            SDL_RenderCopy(Renderer, nameTex, nullptr, &NameRect);
            SDL_DestroyTexture(nameTex);

            if(payCharm.GetMoney() != 0)
                payCharm.onRender(Renderer);
        }

        void RenderCake(SDL_Renderer* Renderer);

        void RenderAddFrame(SDL_Renderer* Renderer);

        void Update(std::vector<Chair>& Chairs,
            int currentTime,
            std::vector<Cabinet>& Cabtines,
            DessertManager dessertManager,
            ProductManager pManager,
            Customer& customer,
            int&  TotalMoney,
            Register& res,
            const std::vector<FurnitureGrid>& grid
        )
        {
            if( x > 800 || x < 0 || y > 600 || y < 0)
            {
                QuitFinish = 1;
            }
            switch(CurrentStage)
            {
                case CustomerStage::Enter:
                {
                    EnterStore(currentTime,(int)Cabtines.size(), grid);
                    break;
                }
                case CustomerStage::Choose:
                {
                    ChooseDessert(Cabtines,currentTime,pManager, grid);
                    break;
                }
                case CustomerStage::Buy:
                {
                    Pay(currentTime,TotalMoney,pManager,Cabtines,res, grid);
                    break;
                }
                case CustomerStage::Eat:
                {
                    Eat(Chairs, currentTime, customer, Cabtines, pManager, grid);
                    break;
                }
                case CustomerStage::Leave:
                {
                    LeaveStore(grid);
                    break;
                }
            }
        }

        void EnterStore(int currentTime, int CabinetSize,
                       const std::vector<FurnitureGrid>& grid)
        {
            // Enter 阶段沿用老办法：硬编码像素路径，不使用 A*
            if(x >= 450)
            {
                toward = 0;
                x = x - speed;
            }
            else if(x < 450 && y <= 520 && x >=200 )
            {
                y = y + speed;
            }
            else if(x >= 200 && x < 450 && y > 520)
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

        void ChooseDessert(std::vector<Cabinet>&Cabinets, int currentTime,
                          ProductManager pManager,
                          const std::vector<FurnitureGrid>& grid)
        {
            if( ChooseTime == 0)
            {
                ChooseTime = currentTime;
            }

            // 首次进入 / 切换橱柜后：计算到橱柜相邻格的路径
            if (path.empty())
            {
                Cabinet& cab = Cabinets[chooseID];
                GridPos cabPos = FurniturePixelToGrid(cab.GetX(), cab.GetY());
                GridPos start = PixelToGrid(x, y);

                // 在橱柜四邻中找距离顾客最近的可行走格子
                const int dr[] = { 0,  0, -1,  1};
                const int dc[] = {-1,  1,  0,  0};
                GridPos bestNeighbor = { -1, -1 };
                int bestDist = 9999;
                for (int i = 0; i < 4; i++)
                {
                    GridPos neighbor = { cabPos.col + dc[i], cabPos.row + dr[i] };
                    if (IsPassableForCustomer(neighbor, grid))
                    {
                        int dist = std::abs(neighbor.col - start.col)
                                 + std::abs(neighbor.row - start.row);
                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            bestNeighbor = neighbor;
                        }
                    }
                }

                if (bestNeighbor.col == -1)
                {
                    // 无相邻可通行格：直接使用橱柜右侧
                    bestNeighbor = { cabPos.col, cabPos.row + 1 };
                }

                path = FindPath(start, bestNeighbor, grid);
                pathIndex = 0;

                if (path.empty())
                {
                    SDL_Log("ChooseDessert: 无路径到橱柜%d", chooseID);
                    x = GridToEntityPixelX(bestNeighbor.row);
                    y = GridToEntityPixelY(bestNeighbor.col);
                }
            }

            // 沿路径走到橱柜旁
            MoveAlongPath();

            // 到达橱柜旁 → 进行微调定位 + 选择逻辑
            if (pathIndex >= path.size())
            {
                Cabinet& cab = Cabinets[chooseID];
                int targetX = cab.GetX() + 32;
                int targetY = cab.GetY() - 16;

                // 像素级微调
                if (x < targetX)      { toward = 1; x += speed; }
                else if (x > targetX) { toward = 0; x -= speed; }
                else if (y < targetY) { toward = 0; y += speed; }
                else if (y > targetY) { toward = 0; y -= speed; }

                if (std::abs(x - targetX) <= speed && std::abs(y - targetY) <= speed)
                {
                    x = targetX;
                    y = targetY;
                    toward = 0;

                    if(Cabinets[chooseID].GetDessertNumber() < ChooseNumber)
                    {
                        SDL_Log("%d %d",Cabinets[chooseID].GetDessertNumber(),ChooseNumber);
                        ChooseNumber = Cabinets[chooseID].GetDessertNumber();
                        eatNumber = ChooseNumber;
                        SDL_Log("由于少于需求，需求数量已更改为%d",ChooseNumber);
                        if( ChooseNumber == 0 && chooseChange <= 3)
                        {
                            SDL_Log("当前橱柜无甜点，切换橱柜");
                            chooseChange++;
                            chooseID = rand() % Cabinets.size();
                            SetChooseNumber();
                            ChooseTime = currentTime;
                            removeCheck = 0;
                            path.clear();  // 重新计算到新橱柜的路径
                            return;
                        }
                        if( chooseChange > 3 )
                        {
                            if(ChooseNumber == 0)
                            {
                                path.clear();
                                CurrentStage = CustomerStage::Buy;
                            }
                        }
                        Cabinets[chooseID].RemoveDessert(ChooseNumber);
                        removeCheck = 1;
                    }
                    if(removeCheck == 0)
                    {
                        Cabinets[chooseID].RemoveDessert(ChooseNumber);
                        removeCheck = 1;
                    }

                    if(currentTime - ChooseTime >= 5000 + rand() % 500 - 250)
                    {
                        int dID = Cabinets[chooseID].GetDessertID();
                        SDL_Log("%s 选择了%d",CustomerName.c_str(), chooseID);
                        int price = pManager.GetProductPrice(dID);
                        SDL_Log("价格为%d",price);
                        payPrice = price * ChooseNumber;
                        path.clear();
                        CurrentStage = CustomerStage::Buy;
                        ChooseTime = 0;
                    }
                }
            }
        }

        void Pay(int CurrentTime, int& TotalMoney, ProductManager& pManager,
                std::vector<Cabinet>& Cabinets, Register& res,
                const std::vector<FurnitureGrid>& grid)
        {
            // 首次进入：计算到收银台附近的路径
            if (path.empty())
            {
                GridPos start = PixelToGrid(x, y);
                GridPos end = { 12, 5 };
                path = FindPath(start, end, grid);
                pathIndex = 0;

                if (path.empty())
                {
                    SDL_Log("Pay: 无路径到收银台");
                    x = GridToEntityPixelX(end.row);
                    y = GridToEntityPixelY(end.col);
                }
            }

            MoveAlongPath();

            // 到达收银台附近 → 排队微调
            if (pathIndex >= path.size())
            {
                if(y >= 150 + Queue * 2)
                {
                    toward = 1;
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
                    }
                    toward = 1;
                    if(CurrentTime - PayTime >= 1000 && isGoingPay == 1 && res.GetIsPaying() == 1)
                    {
                        if(payPrice >= 1000)
                            SDL_Log("warnning!!!!价格超标，数据异常:%d",payPrice);
                        int dID = Cabinets[chooseID].GetDessertID();
                        int price = pManager.GetProductPrice(dID);
                        payPrice = price * ChooseNumber;
                        TotalMoney = TotalMoney + payPrice;

                        payCharm.SetPrice(payPrice);
                        payCharm.SetStartTime(CurrentTime);
                        payCharm.SetStopTime(CurrentTime);

                        SDL_Log("此时%s数据%d,%d",CustomerName.c_str(),ChooseNumber,chooseID);
                        path.clear();
                        CurrentStage = CustomerStage::Eat;
                        PayTime = 0;
                    }
                }
            }
        }

        void Eat(std::vector<Chair>& Chairs, int CurrentTime, Customer& customer,
                std::vector<Cabinet>& Cabinets, ProductManager productManager,
                const std::vector<FurnitureGrid>& grid)
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
                }
                else
                {
                    // 分配了椅子，清空路径让下一帧重新计算
                    path.clear();
                }
            }
            else
            {
                // 首次获得椅子：计算到椅子的路径
                if (path.empty() && onSeat == 0)
                {
                    GridPos start = PixelToGrid(x, y);
                    GridPos chairPos = FurniturePixelToGrid(Chairs[isEating].GetX(), Chairs[isEating].GetY());
                    path = FindPath(start, chairPos, grid);
                    pathIndex = 0;

                    if (path.empty())
                    {
                        SDL_Log("Eat: 无路径到椅子%d", isEating);
                        x = Chairs[isEating].GetRenderX();
                        y = Chairs[isEating].GetRenderY();
                    }
                }

                // 还未入座：沿路径走到椅子旁
                if (onSeat == 0)
                {
                    MoveAlongPath();
                    // for(GridPos i : path)
                    // {
                    //     SDL_Log("路径点: (%d, %d)", i.row, i.col);
                    // }
                    // 路径走完 → 像素级靠近椅子
                    if (pathIndex >= path.size())
                    {
                        int targetX = Chairs[isEating].GetRenderX();
                        int targetY = Chairs[isEating].GetRenderY();

                        if(x - targetX >= 0)
                        {
                            x = x - speed;
                            toward = 0;
                        }
                        else
                        {
                            x = x + speed;
                            toward = 1;
                        }

                        if (std::abs(x - targetX) <= speed)
                        {
                            x = targetX;
                            if(y - targetY >= 0)
                            {
                                y = y - TargetSpeedChangeY(targetY, y, speed);
                            }
                            else
                            {
                                y = y + TargetSpeedChangeY(targetY, y, speed);
                            }
                        }

                        if(x == targetX && y == targetY)
                        {
                            onSeat = 1;
                            if(isEating % 2 == 0)
                                toward = 1;
                            else
                                toward = 0;
                        }
                    }
                }

                // 已入座：等待计时
                if (onSeat == 1)
                {
                    if(CurrentTime - SitTime >= 18000 + rand()% 5000)
                    {
                        Chairs[isEating].SetUsing(0);
                        customer.AddPreference(productManager.products[Cabinets[chooseID].GetDessertID()].GetDessertID(),ChooseNumber);
                        this->preference = customer.GetCustomerPreference();
                        RefreshNameSurface();
                        path.clear();
                        CurrentStage = CustomerStage::Leave;
                        SitTime = 0;
                        whetherRenderAdd = 0;
                    }
                    if( waitingTime == 0 && eatNumber == 0)
                    {
                        waitingTime = CurrentTime;
                    }
                    if( EatTime == 0)
                    {
                        EatTime = CurrentTime;
                    }
                    if( eatNumber == 0 && CurrentTime - waitingTime > 1000)
                    {
                        addNumber = customer.GetAddNumber(productManager.products[Cabinets[chooseID].GetDessertID()].GetDessertID(),ChooseNumber);
                        whetherRenderAdd = 1;
                    }
                    if( eatNumber == 0 && CurrentTime - waitingTime > 3000)
                    {
                        Chairs[isEating].SetUsing(0);
                        whetherRenderAdd = 0;
                        customer.AddPreference(productManager.products[Cabinets[chooseID].GetDessertID()].GetDessertID(),ChooseNumber);
                        this->preference = customer.GetCustomerPreference();
                        RefreshNameSurface();
                        path.clear();
                        CurrentStage = CustomerStage::Leave;
                        SitTime = 0;
                        waitingTime = 0;
                    }
                    if( CurrentTime - EatTime > 1000 && EatTime != 0)
                    {
                        eatNumber--;
                        if( eatNumber < 0)
                            eatNumber = 0;
                        EatTime = 0;
                    }
                }
            }
            if( ChooseNumber == 0 )
            {
                Chairs[isEating].SetUsing(0);
                path.clear();
                CurrentStage = CustomerStage::Leave;
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

        void SetEatNumber( int number );
        int GetEatNumber();

        void LeaveStore(const std::vector<FurnitureGrid>& grid)
        {
            // 首次进入：计算到出口的路径
            if (path.empty())
            {
                GridPos start = PixelToGrid(x, y);
                // 出口在右侧：目标为同一行最右侧边缘格点
                GridPos end = { 12, 22 };
                path = FindPath(start, end, grid);
                pathIndex = 0;

                if (path.empty())
                {
                    SDL_Log("LeaveStore: 无路径到出口，直接传送");
                    x = GridToEntityPixelX(end.row);
                    y = GridToEntityPixelY(end.col);
                }
            }

            // 沿路径走到出口附近
            if (pathIndex < path.size())
            {
                MoveAlongPath();
            }

            // 到达出口格点 → 继续向右走出屏幕
            if (pathIndex >= path.size())
            {
                if (x <= 800)
                {
                    toward = 1;
                    x = x + speed;
                }
                if (x > 800 && y > 350 && CurrentStage == CustomerStage::Leave)
                {
                    QuitFinish = 1;
                }
                if (x > 1000)
                {
                    QuitFinish = 1;
                }
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
                addNumber = 6 * chooseNumber;
            }
            else if(DessertID == PreferDessertID)
            {
                preference = preference + 6 * chooseNumber;
                addNumber = 6 * chooseNumber;
            }
            else
            {
                preference = preference + chooseNumber;
                addNumber = chooseNumber;
            }
            if(preference > 2000)
                preference = 2000;
            SDL_Log("当前喜好值%d",preference);
        }

        int GetAddNumber(int DessertID,int chooseNumber);

        void Clean()
        {
            // NameTexture is created/destroyed each render call now, so don't destroy it here.
            // if(NameSurface)
                // SDL_FreeSurface(NameSurface);
            // if(AddFrameSurface)
            //     SDL_FreeSurface(AddFrameSurface);
            // // NormalTexture is owned by ResourceManager (shared). Do NOT destroy it here.
            // TTF_CloseFont(NameFont);
        }

        // 沿网格路径逐格移动
        void MoveAlongPath()
        {
            if (pathIndex >= path.size()) return;

            GridPos target = path[pathIndex];
            int targetX = GridToEntityPixelX(target.row);
            int targetY = GridToEntityPixelY(target.col);

            if (x < targetX)
            {
                x += speed;
                toward = 1;
            }
            else if (x > targetX)
            {
                x -= speed;
                toward = 0;
            }
            else if (y < targetY)
            {
                y += speed;
            }
            else if (y > targetY)
            {
                y -= speed;
            }

            // 到达当前格点（含容差）
            if (std::abs(x - targetX) <= speed && std::abs(y - targetY) <= speed)
            {
                x = targetX;
                y = targetY;
                pathIndex++;
            }
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

        // 寻路相关
        std::vector<GridPos> path;
        size_t pathIndex = 0;

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
        int EatTime;

        bool hasJoined;
        bool isHovered;
        bool WhetherAppear;
        int Level[10] = {5,20,50,100,200,350,500,700,1000,1500};
        PayCharm payCharm;
        bool onSeat;
        int chooseChange;
        bool removeCheck;

        SDL_Texture* CakeTexture;
        SDL_Texture* PlateTexture;
        SDL_Rect PlateRect;
        SDL_Rect CakeRect;
        SDL_Texture* AddFrameTexture;
        SDL_Rect AddFrameRect;
        SDL_Surface* AddFrameSurface;
        int waitingTime;
        int eatNumber;
        int addNumber;
        bool whetherRenderAdd;
        int offsetY;
};
