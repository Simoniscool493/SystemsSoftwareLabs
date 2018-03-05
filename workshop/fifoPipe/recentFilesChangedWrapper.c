#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "recentFilesChangedWrapper.h"

char* concat(const char *s1, const char *s2);
int IsNotDirectoryPointer(char* fileName);
void PrintLastModifiedResults(char* fileName,char* filePath,int seconds);

void PrintStringArray(char* stringArray[],int length)
{
	for(int i=0;i<length;i++)
	{
		printf("%s\n",stringArray[i]);
	}
}

void GetFilesAfterTimeFromGivenDirectory(char* path,char* modifiedFiles[] ,int* modifiedCount,time_t startTime,char** log,int ignoreTime)
{		
	char* logBuff = "";
	char startBuff[20];
	strftime(startBuff,20,"%Y-%m-%d %H:%M:%S",GetCurrentTimeStructured());
	logBuff = concat(logBuff,"\nChanhelog recorded on ");
	logBuff = concat(logBuff,startBuff);
	logBuff = concat(logBuff,"\n");

	int count,i;
	struct direct **files;
	
	count = scandir(path, &files, file_select, alphasort);
	
	printf("Found %d files in directory.\n",count);
	*modifiedCount = 0;

	if (count == 0)
	{
		printf("No files in Dir\n");
		return;
	}
	else if(count < 0)
	{
		printf("Scandir error code: %d\n",count);
	}

	for (i=0;i<count;++i)
	{
		char* fileName = files[i]->d_name;
		
		if(IsNotDirectoryPointer(fileName))
		{
			char* pathToCheck = concat(path,fileName);
			time_t time = getFileModifiedTime(pathToCheck);
			int diff = difftime(startTime, time);	

			//PrintLastModifiedResults(fileName,pathToCheck,diff);

			if(diff<0 || ignoreTime)
			{		
				modifiedFiles[(*modifiedCount)] = fileName;
				(*modifiedCount)++;
				
				char buff[20];
				strftime(buff,20,"%Y-%m-%d %H:%M:%S",localtime(&time));
				
				logBuff = concat(logBuff,fileName);
				logBuff = concat(logBuff," \n\tLast modified on ");
				logBuff = concat(logBuff,buff);
				logBuff = concat(logBuff," by <TODO>\n");
			}
		}
	}
	
	printf("Found %d modified files in directory. \n",*modifiedCount);
	*(log) = logBuff;
	//printf("Created log message: %s",log);
}

int IsNotDirectoryPointer(char* fileName)
{
	if(strcmp(".",fileName)!=0 && strcmp("..",fileName)!=0)
	{
		return 1;
	}
	
	return 0;
}

void PrintLastModifiedResults(char* fileName,char* filePath,int seconds)
{
	printf("Checking last modified time for filepath: %s\n",filePath);
	
	if(seconds>-1)
	{
		printf("%s has not been modifed since last backup. (seconds is %d)\n",fileName,seconds);
	}
	else
	{
		printf("%s last modified %d seconds ago.\n",fileName,-seconds);
	}
}

int GetFileCountFromGivenDirectory(char* path)
{
	int count,errorCode;
	struct direct **files;

	count = scandir(path, &files, file_select, alphasort);
	
	if(count<0)
	{
		printf("Scandir error code: %d",count);
	}
	
	return count;
}

int file_select(struct direct *entry)
{
	return (1);
}

time_t getFileModifiedTime(char *path) 
{
	struct stat attr;
	int errorCode = stat(path, &attr);
	
	if(errorCode<0)
	{
		printf("stat() error code: %d",errorCode);
	}

	time_t time = attr.st_mtime;

	return time;
}

time_t GetCurrentTimeRaw()
{
	time_t rawtime;
	time( &rawtime );	
	
	return rawtime;
}

struct tm* GetCurrentTimeStructured()
{	
	time_t rawtime;
	time( &rawtime );	

	return localtime(&rawtime);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void CopyFiles(char* files[],int count,char* srcPath,char* destPath)
{
	char* tempPermissions="0000";

	char* sourcePaths[count];
	char* destPaths[count];
	mode_t oldPermissions[count];
	
	for(int i=0;i<count;i++)
	{
		sourcePaths[i] = concat(srcPath,files[i]);
		destPaths[i] = concat(destPath,files[i]);
	}
	
	for(int i=0;i<count;i++)
	{
		char ch;
		FILE* source = fopen(sourcePaths[i], "r");
		remove(destPaths[i]);
		FILE* dest = fopen(destPaths[i], "w");
		
		while(( ch = fgetc(source)) != EOF )
		{
			fputc(ch, dest);
		}

		fclose(source);
		fclose(dest);
	}
}


