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

time_t GetTimeAtRelativeDay(int relativePosiiton,int timeHours,int timeMinutes)
{
	struct tm* now = GetCurrentTimeStructured();
	
	now->tm_mday+=relativePosiiton;
	now->tm_hour = timeHours;
	now->tm_min = timeMinutes;
	
	//char buff[20];
	//strftime(buff, sizeof(buff), "%b %d %H:%M", now);
	//printf("Time created is %s\n",buff);
	
	return mktime(now);
}

int IsTimeAfter(time_t time1,time_t time2)
{
	if(difftime(time1,time2)>0)
	{
		return 1;
	}
	
	return 0;
}


