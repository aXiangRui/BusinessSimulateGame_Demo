#pragma once

#include"RUI_Customer.h"
#include"RUI_Clock.h"
#include<vector>
#include<string>

class GameEvent
{
    public:
    GameEvent() = default;
    ~GameEvent() = default;

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
        for(int i = 0; i < Customers.size(); i++)
        {
            if(Customers[i].GetCustomerID() == id)
            {
                SwapCustomer(Customers[i],Customers[Customers.size()-1]);
                Customers.pop_back();
            }
        }
    }

    void SetClock(Clock c)
    {
        timeClock = c;
    }

    void onUpdate()
    {
        CurrentTime = SDL_GetTicks();
        if(timeClock.ReturnHour()>=6 && timeClock.ReturnHour() < 22)
        {
            if(CurrentTime - LastTime >= 1000 + (rand()%1000) - 500)
            {
                int j = rand() % 4;
                if(j <= 3)
                {
                    Customer a;
                    a.InitCustomer(Customers.size(),0,"testcustomer");
                    a.SetEnterTime(CurrentTime);
                    AddCustomer(a);
                    SDL_Log("增加顾客，当前%d人",Customers.size());
                }
                LastTime = CurrentTime;
            }          
        }
        for(int i = Customers.size() - 1; i >= 0; i--) 
        {
            // if(CurrentTime - LastTime >= 1000)
            //     SDL_Log("%d",Customers[i].GetCustomerID());
            if(CurrentTime - Customers[i].GetEnterTime() >= 10000) {
                DeleteCustomer(Customers[i].GetCustomerID());  // 删除下标i的元素
                SDL_Log("顾客离开，剩下%d人", Customers.size());
            }
        }
    }
    private:
        std::vector<Customer>Customers;
        Clock timeClock;
        Uint32 CurrentTime;
        Uint32 LastTime;
};