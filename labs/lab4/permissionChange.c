#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{    
	char mode[]="0777";
	char fileName[100]="fileToChange";
	int i;
	i = strtol(mode,NULL,8);
	if (chmod (fileName,i) < 0)
	{
		printf("error in chmod");
	}


}
