#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main()
{
	printf("Program 2: Child process\n");

	time_t rawtime;
	struct tm *timeToRing;
	time( &rawtime );
	timeToRing = localtime( &rawtime );

	printf("Time 1 is %d:%d:%d\n", timeToRing->tm_hour,timeToRing->tm_min,timeToRing->tm_sec);
		
	sleep(2);

	time_t rawtime2;
	struct tm *timeToRing2;
	time( &rawtime2 );
	timeToRing2 = localtime( &rawtime2 );

	printf("Time 2 is %d:%d:%d\n", timeToRing2->tm_hour,timeToRing2->tm_min,timeToRing2->tm_sec);
	
	int diftime = difftime(rawtime,rawtime2);
	printf("Diftime is %d\n", diftime);
	return 0;
}
