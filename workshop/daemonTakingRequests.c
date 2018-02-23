#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

time_t GetRawTime();
struct tm* GetStructuredTime();
void SetToDaemon();

int main()
{
	//printf("Starting\n");
	int pid = fork();
	time_t startTime = GetRawTime();  

	printf("Forked with PID: %d\n",pid);
	
	if(pid > 0)
	{
		sleep(1);
		//printf("1 is exiting\n");
		exit(EXIT_SUCCESS);
	}
	else if(pid ==0)
	{
		SetToDaemon();
		
		//printf("2 has elevated and is entering loop\n");
		
		int i = 0;
		while(1)
		{
			int seconds = difftime(GetRawTime(), startTime);
			printf("%d\n",seconds);
			fflush(stdout);
			sleep(1);
			
			if(seconds>10)
			{
				exit(0);
			}
			
		}
	}
	
	return 0;
}

time_t GetRawTime()
{
	time_t rawtime;
	time( &rawtime );	
	
	return rawtime;
}

struct tm* GetStructuredTime()
{	
	time_t rawtime;
	time( &rawtime );	

	return localtime(&rawtime);
}

void SetToDaemon()
{
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
}
