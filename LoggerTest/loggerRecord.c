#include <stdio.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/file.h>
#include <poll.h>
#include <syslog.h>
#include <string.h>

int main()
{
	setuid(geteuid());

	FILE * fp = fopen("/var/log/audit/audit.log", "r");
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	if (fp < 0)
	{
		printf("File error code: %d\n",fp);
		exit(EXIT_FAILURE);
	}
	
	int count = 0;

	while ((read = getline(&line, &len, fp)) != -1) 
	{
		if(
			(strstr(line,"/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website") != NULL) &&
		    (strstr(line," auid=") != NULL)
		)
		{
			printf("%s", line);
			count++;
		}
	}

	fclose(fp);
	if (line)
	{
		free(line);
	}
	
	printf("Total count: %d\n",count);

	return 0;
}
