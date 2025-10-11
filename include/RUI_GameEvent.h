#pragma once

#include"RUI_Customer.h"
#include"RUI_Clock.h"
#include<vector>
#include<string>
#include<fstream>
#include<sstream>

class GameEvent
{
    public:
    GameEvent() = default;
    ~GameEvent() = default;

    int test = 0;

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
                SDL_Log("%d",id);
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
                    a.InitCustomer(test,0,"testcustomer");
                    a.SetEnterTime(CurrentTime);
                    AddCustomer(a);
                    SDL_Log("增加顾客，当前%d人",Customers.size());
                    test++;
                }
                LastTime = CurrentTime;
            }          
        }
        for(int i = Customers.size() - 1; i >= 0; i--) 
        {
            // if(CurrentTime - LastTime >= 1000)
            //     SDL_Log("%d",Customers[i].GetCustomerID());
            if(CurrentTime - Customers[i].GetEnterTime() >= (10000 + rand()%5000 - 2500)) {
                DeleteCustomer(Customers[i].GetCustomerID());  // 删除下标i的元素
                SDL_Log("顾客离开，剩下%d人", Customers.size());
            }
        }
    }

    int ReturnClockTime()
    {
        return timeClock.ReturnAllHour();
    }

    void Load()
    {
        std::ifstream file("./save/test.txt");
        if(!file)
        {
            SDL_Log("Load: failed to open save file");
            return;
        }
        std::vector<int> tokens;
        std::string line;
        while(std::getline(file, line))
        {
            std::istringstream iss(line);
            int v;
            while(iss >> v)
                tokens.push_back(v);
        }
        file.close();

        if(tokens.empty())
        {
            SDL_Log("Load: no data in save file");
            return;
        }
        timeClock.SetClockTime(tokens[0]);
        for(size_t k = 1; k + 1 < tokens.size(); k += 2)
        {
            int id = tokens[k];
            int ent = tokens[k+1];
            Customer a;
            a.InitCustomer(id, 0, "test");
            a.SetEnterTime(ent);
            Customers.push_back(a);
        }
    }

    void Save()
    {
        std::ofstream file("./save/test.txt");
        if(!file)
        {
            SDL_Log("Save: failed to open save file for writing");
            return;
        }
        file << timeClock.ReturnAllHour() << std::endl;
        for(size_t i = 0; i < Customers.size(); ++i)
        {
            file << Customers[i].GetCustomerID() << " " << Customers[i].GetEnterTime() << std::endl;
        }
        file.close();
    }

    private:
        std::vector<Customer>Customers;
        Clock timeClock;
        Uint32 CurrentTime;
        Uint32 LastTime;
};