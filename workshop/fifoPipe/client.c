#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include <unistd.h>

#include "fifoWrapper.h"

int main()
{
	int fifo_server,dataToSend,errorCode;

	printf("Welcome to the daemon control utility. Please choose an option:\n \
			1: Start a backup and update.\n \
			2: Record a changelog.\n \
			3: Start a backup.\n \
			4: Start an update.\n \
			5: Shut down server.\n");
			
	while(1)
	{
		scanf("%d",&dataToSend);
		
		if(dataToSend >0 && dataToSend<6) {break;}

		printf("Please enter either 1,2,3,4, or 5.\n");
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



