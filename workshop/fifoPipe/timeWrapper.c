#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "timeWrapper.h"


time_t GetCurrentTimeRaw()
{
	time_t rawtime;
	time( &rawtime );	
	
	return rawtime;
}

struct tm* GetCurrentTimeStructured()
{	
	time_t rawtime;
	time( &rawtime );	

	return localtime(&rawtime);
}

time_t GetTimeAtRelativeDay(int relativePosiiton,int timeHours,int timeMinutes,int timeSeconds)
{
}

int IsTimeAfter(time_t time1,time_t time2)
{
}

