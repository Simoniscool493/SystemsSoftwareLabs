#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	int pid = fork();

	if(pid > 0)
	{
		sleep(15);
		exit(EXIT_SUCCESS);
	}
	else if(pid ==0)
	{
		time_t rawtime;
		struct tm *timeToRing;
		time( &rawtime );
		timeToRing = localtime( &rawtime );		
		
		int hours,minutes,seconds;
		
		printf("Enter hours\n");
		scanf("%d",&hours);
		fflush(stdout);	
		
		printf("Enter minutes\n");
		scanf("%d",&minutes);
		fflush(stdout);		
		
		printf("Enter seconds\n");
		scanf("%d",&seconds);
		fflush(stdout);
		
		timeToRing->tm_hour = hours;
		timeToRing->tm_min = minutes;
		timeToRing->tm_sec = seconds;
		rawtime = mktime (timeToRing);
		
		printf("Time to ring is %d:%d:%d\n", timeToRing->tm_hour,timeToRing->tm_min,timeToRing->tm_sec);

		if(setsid() < 0)
		{
			printf("Program 2: Error: Could not elevate\n");
			exit(EXIT_FAILURE);
		}

		umask(0);
		
		if(chdir("/") < 0)
		{
			printf("Program 2: Error: Could not set wd to root\n");
			exit(EXIT_FAILURE);
		}

		while(1)
		{
			sleep(1);
			
			time_t rawtime2;
			struct tm *info;
			time( &rawtime2 );
			info = localtime( &rawtime2 );
			
			int difTime = difftime(rawtime,rawtime2);
			
			if(difTime<0)
			{
				printf("Program 2: Ring ring!\n");
			}
		}
	}
	
	return 0;
}
