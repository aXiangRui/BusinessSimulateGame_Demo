#pragma once

#include"RUI_Customer.h"
#include"RUI_Clock.h"
#include"RUI_Cabinet.h"
#include"RUI_CustomerManager.h"
#include"RUI_DessertManager.h"
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<queue>

class GameEvent
{
    public:
    GameEvent() = default;
    ~GameEvent() = default;

    int test = 0;
    std::queue<int> payQueue;

    void AddCustomer(Customer cus)
    {
        Customers.push_back(cus);
    }

    void SwapCustomer(Customer& a, Customer& b)
    {
        Customer c = a;
        a = b;
        b = c;
    }

    void DeleteCustomer(int id)
    {
        for(size_t i = 0; i < Customers.size(); i++)
        {
            if(Customers[i].GetCustomerID() == id)
            {
                SwapCustomer(Customers[i], Customers[Customers.size()-1]);
                Customers.pop_back();
                SDL_Log("删除顾客: removed id=%d", id);
                return; // stop after removing to avoid invalid indices
            }
        }
    }

    void SetClock(Clock c)
    {
        timeClock = c;
    }

    void onUpdate(std::vector<Chair>& Chairs,
        std::vector<Cabinet>& Cabinets,
        CustomerManager customerManager,
        DessertManager dessertManager,
        int& TotalMoney
        )
    {
        if(test >= 10000000)
        {
            test = 0;
            //防溢出，不过基本上也没人能招待这么多吧...
        }
        CurrentTime = SDL_GetTicks();
        if(timeClock.ReturnHour()>=6 && timeClock.ReturnHour() < 22)
        {
            if(CurrentTime - LastTime >= 2500 + (rand()%2000) - 1000)
            {
                int j = rand() % 4;
                if(j <= 3)
                {
                    int randIndex = rand()%customerManager.GetCustomersSize();
                    Customer a;
                    a.InitCustomer(
                        test,
                        customerManager.GetPreferDessertID(randIndex),
                        customerManager.GetCustomerName(randIndex),
                        customerManager.GetCustomerPath(randIndex),
                        customerManager.GetCustomerPreference(randIndex)
                    );
                    AddCustomer(a);
                    SDL_Log("增加顾客，当前%d人",Customers.size());
                    test++;
                }
                LastTime = CurrentTime;
            }          
        }
        for(int i = (int)Customers.size() - 1; i >= 0; --i)
        {    
            Customers[i].Update(Chairs, CurrentTime, Cabinets, dessertManager ,TotalMoney);
            //差点找不到顾客类刷新了哈哈哈哈
            if(Customers[i].GetQuit() && Customers[i].getX() > 800 && Customers[i].getY() > 350) {
                SDL_Log("顾客准备离开，此时id:%d,x:%d,y:%d",Customers[i].GetCustomerID(),Customers[i].getX(),Customers[i].getY());
                DeleteCustomer(Customers[i].GetCustomerID());  // 删除特定 id 的元素 (DeleteCustomer 会返回)
                SDL_Log("顾客离开，剩下%d人", (int)Customers.size());
            }
        }
        for(int i = 0; i < Customers.size(); i++)
        {
            if(Customers[i].isGoingToPay(CurrentTime))
            {
                Customers[i].setIsGoingToPay(1);
                payQueue.push(Customers[i].GetCustomerID());
            }
        }

        // 更新队列中的位置（Queue），通过 id 查找对应的 Customers 元素，避免把 id 当作 vector 索引
        
        std::queue<int> tmp = payQueue;
        int pos = 0;
        while(!tmp.empty())
        {
            int id = tmp.front(); tmp.pop();
            int idx = -1;
            for (int k = 0; k < (int)Customers.size(); ++k)
            {
                if (Customers[k].GetCustomerID() == id) { idx = k; break; }
            }
            if (idx != -1)
            {
                Customers[idx].SetQueueNumber(pos);
            }
            pos++;
        }
        

        // 只处理队首顾客
        if (!payQueue.empty())
        {
            int frontId = payQueue.front();
            int idx = -1;
            for (int k = 0; k < (int)Customers.size(); ++k)
            {
                if (Customers[k].GetCustomerID() == frontId) { idx = k; break; }
            }
            if (idx == -1)
            {
                // 顾客已被删除，安全弹出
                payQueue.pop();
            }
            else
            {
                Customer &front = Customers[idx];
                // 清除其它顾客的队首标志，确保只有当前队首被标记
                for (int k = 0; k < (int)Customers.size(); ++k)
                {
                    if (k == idx) continue;
                    Customers[k].setIsfront(false);
                }
                front.setIsfront(true);
                // 只有队首才设置PayTime
                if (front.GetPayTime() == 0) front.SetPayTime(CurrentTime);
                if (front.hasPayed())
                {
                    front.setIsfront(false);
                    payQueue.pop();
                }
            }
        }
    }

    void onRender(SDL_Renderer* Renderer)
    {
        for(int i = 0;i < Customers.size(); i++)
        {
            Customers[i].OnRender(Renderer);
        }
    }

    int ReturnClockTime()
    {
        return timeClock.ReturnAllHour();
    }

    void Load()
    {
        std::ifstream file("./save/Time.txt");
        std::string string;
        int line = 0;
        int time;
        while(std::getline(file,string))
        {
            line++;
            if(line == 1)
            {
                file >> time;
                test = time;
            }
            else if(line >= 3)
            { 
                std::istringstream iss(string);
                int cID;
                std::string cPath;
                iss >> cID >> cPath;
                Customer a;
                a.InitCustomer((int)cID, 0, "cName", cPath.c_str(), 0);
                Customers.push_back(a);
            }
        }
        file.close();

        std::ifstream file01("./save/Total.txt");
        file01 >> test;
        SDL_Log("读取到总人数为%d",test);
        file.close();
    }

    void Save()
    {
        std::ofstream file("./save/Time.txt");
        if(!file)
        {
            SDL_Log("Save: failed to open save file for writing");
            return;
        }
        file << timeClock.ReturnAllHour() << std::endl;

        for(size_t i = 0; i < Customers.size(); ++i)
        {
            file << Customers[i].GetCustomerID() << " " << Customers[i].GetCustomerPath() << std::endl;
        }
        file.close();

        std::ofstream file01("./save/Total.txt");
        file01 << test <<std::endl;
        file01.close();
    }

    private:
        std::vector<Customer>Customers;
        Clock timeClock;
        Uint32 CurrentTime;
        Uint32 LastTime;
};