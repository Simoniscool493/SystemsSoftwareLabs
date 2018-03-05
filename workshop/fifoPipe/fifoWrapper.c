#include<stdio.h> 
#include<fcntl.h>
#include <unistd.h>
#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>

int ReadFIFO(int fileHandle)
{
	int data,errorCode;
	errorCode = read(fileHandle,&data,sizeof(int));
	
	if(errorCode<0)
	{
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
		printf("FIFO open error code: %d\n",fileHandle);
	}
	
	return fileHandle;
}

int CreateFIFO(const char* path)
{
	int fileHandler = mkfifo(path,0666); 

	if(fileHandler<0) 
	{
		printf("FIFO create error code: %d\n",fileHandler);
		exit(-1);
	}
	
	return fileHandler;
}
