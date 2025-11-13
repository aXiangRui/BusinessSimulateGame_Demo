#include "../include/RUI_Product.h"

int CreateDessertID(int baseid, int* type)
{
    switch(baseid)
    {
        case 0:
        {
            if(type[0] == 2 && type[1] == 1)
                return 1;
            if(type[0] == 1 && type[2] == 2)
                return 2;
            if(type[0] == 1 && type[4] == 1 && type[5] == 1)
                return 7;
            if(type[2] == 1 && type[3] == 1 && type[6] == 1)
                return 8;
            break;
        }
        case 3:
        {
            if(type[0] == 3)
                return 4;
            if(type[0] == 1 && type[1] == 2)
                return 5;
            if(type[0] == 1 && type[2] == 2)
                return 6;
        }
        default:break;
    }
    return baseid;
}
/*材料Type: | 蛋糕Type:
--0--奶油   | ----0----戚风----1----草莓
--1--草莓   | ----2--巧克力----3----果冻
--2--巧克力 | -4-奶油冰激凌--5-草莓冰激凌
--3--樱桃   | -6-巧克力冰激凌-7-提拉米苏
--4--咖啡   | ----8--黑森林
--5--饼干   |
--6--果汁   |
*/
