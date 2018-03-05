#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include <unistd.h>

#include "fifoWrapper.h"

int main()
{
	int fifo_server,dataToSend,errorCode;

	printf("Welcome to the daemon control utility. Please choose an option:\n \
			1: Start a backup/update.\n \
			2: Record a changelog.\n \
			4: Shut down server.\n");
			
	while(1)
	{
		scanf("%d",&dataToSend);
		
		if(dataToSend >0 && dataToSend<5) {break;}

		printf("Please enter either 1,2,3 or 4.\n");
	}
	
	printf("You have selected option %d.",dataToSend);
	
	fifo_server=OpenFIFO("fifo_server");
	
	errorCode = write(fifo_server,&dataToSend,sizeof(int));
	
	if(errorCode<0)
	{
		printf("FIFO write error code: %d\n",errorCode);
	}
	
	errorCode = close(fifo_server);
	
	if(errorCode<0)
	{
		printf("FIFO close error code: %d\n",errorCode);
	}
	
	printf("Finished.\n");
}


