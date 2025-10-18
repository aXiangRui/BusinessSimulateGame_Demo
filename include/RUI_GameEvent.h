#pragma once

#include"RUI_MenuButton.h"
#include"RUI_Customer.h"
#include"RUI_Clock.h"
#include"RUI_Cabinet.h"
#include"RUI_CustomerManager.h"
#include"RUI_DessertManager.h"
#include"RUI_ProductManager.h"
#include"RUI_BaseMaterial.h"
#include"RUI_MaterialManager.h"
#include"RUI_ChatFrame.h"
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

    std::queue<int> payQueue;
    std::vector<Customer> AddCustomers;
    ProductManager productManager;
    std::vector<Plate> plates;
    std::vector<MenuButton>Btns;
    DessertManager dessertManager;
    MaterialManager materialManager;
    ChatFrame Chat;

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
                // 从队列中移除该 id，防止留下空洞
                RemoveIdFromQueue(id);
                return; // stop after removing to avoid invalid indices
            }
        }
    }

    void onEnter()
    {
        dessertManager.InitDessertManager();
        materialManager.InitMaterialManager();    
        productManager.InitProductManager();
        
        for(int i = 0; i < 3; i++)
        {
            Plate a;
            a.InitPlate(200,200,i);
            plates.push_back(a);
        }
        MenuButton Btn0((WindowWidth-320)/2,450,320,64,"设置新甜点",0);
        Btns.push_back(Btn0);
    }

    void quit()
    {
        productManager.quit();
        dessertManager.quit();
        dessertManager.quit();
        plates.clear();
        Btns.clear();
    }

    void RemoveIdFromQueue(int id)
    {
        if (payQueue.empty()) return;
        std::queue<int> newQ;
        while(!payQueue.empty())
        {
            int v = payQueue.front(); payQueue.pop();
            if (v != id) newQ.push(v);
        }
        payQueue = std::move(newQ);
    }

    void SetClock(Clock c)
    {
        timeClock = c;
    }

    void input(const SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                break;
            }
        }
    }

    void onUpdate(std::vector<Chair>& Chairs,
        std::vector<Cabinet>& Cabinets,
        CustomerManager& customerManager,
        DessertManager dessertManager,
        int& TotalMoney,
        int& TotalCustomers
        )
    {
        if(customerManager.GetCustomersSize() > 4)
        {
        for(int i = 0; i < customerManager.GetCustomersSize(); i++)
                {
                    SDL_Log("第%d人，%s",i,customerManager.Customers[i].GetCustomerName().c_str());
                }
        }
        
        if(TotalCustomers >= 10000000)
        {
            TotalCustomers = 0;
            //防溢出，不过基本上也没人能招待这么多吧...
        }
        CurrentTime = SDL_GetTicks();
        for(int i = 0; i < AddCustomers.size(); i++)
        {
            if(CurrentTime - LastTime >= AddCustomers[i].GetDelayTime())
            {
                AddCustomer(AddCustomers[i]);     
                std::string name = AddCustomers[i].GetCustomerName();
                SwapCustomer(AddCustomers[i],AddCustomers[AddCustomers.size()-1]);
                AddCustomers.pop_back();
            }
        }
        if(timeClock.ReturnHour() >= 7 && timeClock.ReturnHour() < 21)
        {
            if(CurrentTime - LastTime >= 7000 + (rand()%2000) - 1000)
            {
                for(int i = 0; i < customerManager.GetCustomersSize(); i++)
                {
                    if(customerManager.Customers[i].WhetherAdd(Customers.size(),timeClock))
                    {
                        // SDL_Log("可以生成%d",i);
                        Customer a;
                        a.InitCustomer(
                            TotalCustomers,
                            customerManager.GetPreferDessertID(i),
                            customerManager.GetCustomerName(i),
                            customerManager.GetCustomerPath(i),
                            customerManager.GetCustomerPreference(i)
                        );
                        AddCustomers.push_back(a);
                        customerManager.Customers[i].SetHasJoined(1);
                        TotalCustomers++;
                        
                        LastTime = CurrentTime;
                    }

                }
            }          
        }
        for(int i = (int)Customers.size() - 1; i >= 0; --i)
        {    
            Customers[i].Update(Chairs, CurrentTime, Cabinets, dessertManager, productManager, TotalMoney);
            //差点找不到顾客类刷新了哈哈哈哈
            if(Customers[i].GetQuit() && Customers[i].getX() > 800 && Customers[i].getY() > 350) 
            {
                SDL_Log("顾客准备离开，此时id:%d,x:%d,y:%d",Customers[i].GetCustomerID(),Customers[i].getX(),Customers[i].getY());
                std::string name = Customers[i].GetCustomerName();
                for(int i = 0; i < customerManager.GetCustomersSize(); i++)
                {
                    if(name == customerManager.Customers[i].GetCustomerName())
                    {
                       customerManager.Customers[i].SetHasJoined(0);
                    }
                }
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
        

        // 只处理队首顾客（先清理队列中不存在的 id）
        while(!payQueue.empty()) {
            int peekId = payQueue.front();
            bool exists = false;
            for (int k = 0; k < (int)Customers.size(); ++k) {
                if (Customers[k].GetCustomerID() == peekId) { exists = true; break; }
            }
            if (!exists) {
                payQueue.pop();
            } else break;
        }

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
                // 顾客已被删除，安全弹出（防御）
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
        //productManager.products[0].onRender(Renderer,dessertManager,materialManager,plates,
        //200,200,200,200);
    }

    void onFrameRender(SDL_Renderer* Renderer, CabinetFrame cabinetFrame, std::vector<Cabinet> Cabinets)
    {
        int cid = cabinetFrame.GetCabinetID();
        if( cid >= 0 && cid < Cabinets.size())
        {
            cabinetFrame.onRender(Renderer, Cabinets[cid],productManager);
            Btns[0].ButtonRender(Renderer);
        }    
    }

    void onProductRender(SDL_Renderer* Renderer)
    {
        Chat.RenderFrame(Renderer);
        for(int i = 0; i < productManager.GetProductSize(); i++)
        {
            if(i / 6 == 0)
            {
                productManager.products[i].onRender(
                    Renderer, dessertManager, materialManager, plates,
                    200 + i/3 * 200, i % 3 * 200, 200, 200
                );
            }
        }
    }

    int ReturnClockTime()
    {
        return timeClock.ReturnAllHour();
    }

    int GetCustomerNumber()
    {
        return Customers.size();
    }

    void Load(int& TotalMoney, int& TotalCustomers)
    {
        std::ifstream file("./save/Time.txt");
        std::string string;
        int time;
        file >> time;
        timeClock.SetClockTime(time);
        int line = 0;
        while(std::getline(file,string))
        {
            line++;
            if(line >= 2)
            { 
                std::istringstream iss(string);
                int cID;
                std::string cPath;
                iss >> cID >> cPath;
                Customer a;
                a.InitCustomer((int)cID, 0, "cName", cPath.c_str(), 0);
                bool overload = 0;
                for(int i = 0; i < Customers.size(); i++)
                {
                    if(Customers[i].GetCustomerID() == cID)
                    overload = 1;
                }
                if(!overload)
                    Customers.push_back(a);
            }
        }
        file.close();

        std::ifstream file01("./save/Total.txt");
        file01 >> TotalCustomers;
        file01 >> TotalMoney;
        SDL_Log("读取到总人数为%d",TotalCustomers);
        file01.close();
    }

    void Save(int& TotalMoney, int& TotalCustomers)
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
        file01 << TotalCustomers <<std::endl;
        file01 << TotalMoney << std::endl;
        file01.close();
    }

    private:
        std::vector<Customer>Customers;
        Clock timeClock;
        Uint32 CurrentTime;
        Uint32 LastTime;

};