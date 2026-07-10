#pragma once

#include <cmath>
#include <functional>
#include <SDL2/SDL.h>

int TargetSpeedChangeX(int targetX, int mx, int speed);

int TargetSpeedChangeY(int targetY, int my, int speed);

struct RenderCommand
{
    int y;
    int type;
    int index;
};