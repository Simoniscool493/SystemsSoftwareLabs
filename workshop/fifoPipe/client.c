#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include "fifoWrapper.h"

int main()
{
	int fifo_server,dataToSend,errorCode;
	openlog("SystemsSoftwareAssignmentProgramClient", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

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
		syslog(LOG_USER, "FIFO write error code: %d\n",errorCode);
		printf("FIFO write error code: %d\n",errorCode);
	}
	
	errorCode = close(fifo_server);
	
	if(errorCode<0)
	{
		syslog(LOG_USER, "FIFO close error code: %d\n",errorCode);
		printf("FIFO close error code: %d\n",errorCode);
	}
	
	printf("Finished.\n");
}




