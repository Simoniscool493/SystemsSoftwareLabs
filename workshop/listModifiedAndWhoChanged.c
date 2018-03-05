#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int file_select();
time_t getFileModifiedTime();
extern int alphasort();
void GetFilesFromCurrentWd(char* modifiedFiles[] ,int* modifiedCount);
time_t GetRawTime();
struct tm* GetStructuredTime();
int GetWdCount();
void PrintStringArray(char* stringArray[]);

int main() 
{
	char* files[GetWdCount()];
	int count;

	GetFilesFromCurrentWd(files,&count);
	
	PrintStringArray(files);
}

void PrintStringArray(char* stringArray[])
{
	for(int i=0;stringArray[i]!=NULL;i++)
	{
		printf("%s\n",stringArray[i]);
	}
}

void GetFilesFromCurrentWd(char* modifiedFiles[] ,int* modifiedCount)
{		
	int count,i;
	struct direct **files;
	time_t startTime = GetRawTime();
	sleep(5);

	char pathname[MAXPATHLEN];

	getcwd(pathname,MAXPATHLEN);
	
	count = scandir(pathname, &files, file_select, alphasort);

	*modifiedCount = 0;

	if (count <= 0)
	{
		printf("No files in Dir\n");
		exit(0);
	}

	for (i=1;i<count+1;++i)
	{
		if(strcmp(".",files[i-1]->d_name)!=0 && strcmp("..",files[i-1]->d_name)!=0)
		{
			time_t time = getFileModifiedTime(files[i-1]->d_name);
			int diff = difftime(startTime, time);		

			if(diff<0)
			{		
				modifiedFiles[(*modifiedCount)] = files[i-1]->d_name;
				(*modifiedCount)++;
			}
		}
	}
}

int GetWdCount()
{
	int count;
	struct direct **files;
	char pathname[MAXPATHLEN];

	getcwd(pathname,MAXPATHLEN);
	count = scandir(pathname, &files, file_select, alphasort);
	
	return count;
}

int file_select(struct direct *entry)
{
	return (1);
}

time_t getFileModifiedTime(char *path) 
{
	struct stat attr;
	stat(path, &attr);

	time_t time = attr.st_mtime;

	return time;
}

time_t GetRawTime()
{
	time_t rawtime;
	time( &rawtime );	
	
	return rawtime;
}

struct tm* GetStructuredTime()
{	
	time_t rawtime;
	time( &rawtime );	

	return localtime(&rawtime);
}
