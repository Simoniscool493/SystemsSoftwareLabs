#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include "fifoWrapper.h"

int main()
{	
	CreateFIFO("fifo_server");
	printf("Server created\n");
}

