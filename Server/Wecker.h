#pragma once

#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <string.h>

class Wecker
{

public:
    static Wecker& Get()
    {
        static Wecker WeckerInstanz;
        return WeckerInstanz;
    }
    Wecker(const Wecker&) = delete;

    static bool InitWecker();

    static timer_t GetTimerID(){return Get().timerID;};

    static void SetTimerID(timer_t newTimerID){Get().timerID = newTimerID ;};

    static bool SetWeckzeit(std::string);

    static bool AddDelayTime(std::string);

    static bool GetTimerExpiration();

private:
    Wecker() {}
    timer_t timerID;

};