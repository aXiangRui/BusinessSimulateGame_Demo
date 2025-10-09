#pragma once

class Clock
{
    public:
        Clock() = default;
        ~Clock() = default;

    int ClockTime;
    void SetStartTime(int i)
    {
        ClockTime = i;
    }

    void UpdateTime()
    {
        ClockTime = ClockTime + 1;
    }

    int ReturnHour()
    {
        return ClockTime % 24;
    }

    int ReturnDay()
    {
        return ClockTime / 24;
    }

};