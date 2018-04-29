#include<stdio.h> 
#include<fcntl.h>
#include <unistd.h>
#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<syslog.h>

int ReadFIFO(int fileHandle)
{
	int data,errorCode;
	errorCode = read(fileHandle,&data,sizeof(int));
	
	if(errorCode<0)
	{
		syslog(LOG_USER, "FIFO read error code: %d\n",errorCode);
		printf("FIFO read error code: %d\n",errorCode);
		exit(-1);
	}
	
	sleep(1);
	
	return data;
}

int OpenFIFO(const char* path)
{
	//printf("Opening file: \"%s\"...\n",path);
	int fileHandle = open(path,O_RDWR);

	if(fileHandle<1) 
	{
		syslog(LOG_USER, "FIFO open error code: %d\n",fileHandle);
		printf("FIFO open error code: %d\n",fileHandle);
	}
	
	return fileHandle;
}

int CreateFIFO(const char* path)
{
	int fileHandler = mkfifo(path,0666); 

	//No error code here because mkfifo fails if the fole already exists, which will be true the majority of the time

	if(fileHandler<0) 
	{
		exit(0);
	}
	
	return fileHandler;
}



