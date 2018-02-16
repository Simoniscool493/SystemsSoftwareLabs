#include <stdio.h>
#include <stdlib.h>

int main()
{
	int pid = fork();
	
	if(pid > 0)
	{
		printf("Parent process");
		sleep(5);
		exit(EXIT_SUCCESS);
	}
	else if(pid ==0)
	{
		printf("Child process");
		
		if(setsid() < 0)
		{
			printf("Error: Could not elevate");
			exit(EXIT_FAILURE);
		}
		
		umask(0);
		
		if(chdir("/") < 0)
		{
			printf("Error: Could not set wd to root");
			exit(EXIT_FAILURE);
		}
		
		while(1)
		{
			sleep(1);
			printf("Daemon: my parent is: %i\n", getppid());
		}
	}
	
	return 0;
}
