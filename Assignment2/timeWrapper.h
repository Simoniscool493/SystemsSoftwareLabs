#ifndef TIMEWRAPPER_H
#define TIMEWRAPPER_H

time_t GetCurrentTimeRaw();
struct tm* GetCurrentTimeStructured();
time_t GetTimeAtRelativeDay(int relativePosiiton,int timeHours,int timeMinutes);
int IsTimeAfter(time_t time1,time_t time2);

#endif


