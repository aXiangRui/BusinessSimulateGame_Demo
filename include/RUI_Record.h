#pragma once

#include <vector>

class Record
{
    public:
    Record() = default;
    ~Record() = default;

    void Load()
    {

    }
    private:
    const int AllTotal;
    int DailyTotal;
    std::vector<int>dessertNumber;
    int AllDessertNumber;
    int CustomerNumber;
    int AllCustomerNumber;
    int AllCost;
    int DailyCost;
};