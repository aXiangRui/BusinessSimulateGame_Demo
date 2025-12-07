#include "../include/RUI_Dessert.h"

int RemoveSmallCakesByID(std::vector<SmallCake>& smallcakes, int targetID)
{
    int sum = 0;
    for(int i = 0; i < smallcakes.size(); i++)
    {
        if(smallcakes[i].GetID() == targetID)
        {
            sum++;
            smallcakes.erase(smallcakes.begin() + i);
        }
    }
    return sum;
}