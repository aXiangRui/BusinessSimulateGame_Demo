#include "../include/RUI_Tool.h"

#include <algorithm>
#include <cmath>

int TargetSpeedChangeX(int targetX, int mx, int speed)
{
    return abs( targetX - mx ) >= speed? speed : abs( targetX - mx);
}

int TargetSpeedChangeY(int targetY, int my, int speed)
{
    return abs(targetY - my) >= speed? speed : abs( targetY - my);
}