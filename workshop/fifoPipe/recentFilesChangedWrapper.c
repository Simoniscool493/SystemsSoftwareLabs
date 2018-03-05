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
#include "timeWrapper.h"

char* concat(const char *s1, const char *s2);
int IsNotDirectoryPointer(char* fileName);
void PrintLastModifiedResults(char* fileName,char* filePath,int seconds);
char* LastIndexSubStr(char* stringToSearch,char charToGetLastOf);

void PrintStringArray(char* stringArray[],int length)
{
	for(int i=0;i<length;i++)
	{
		printf("%s\n",stringArray[i]);
	}
}

void GetFilesFromGivenDirectory(char* path,char** files,int* fileCount)
{
	int count,i;
	struct direct **filesStruct;
	count = scandir(path, &filesStruct, file_select, alphasort);
	
	if(count<0)
	{
		printf("Scandir error code: %d\n",count);
		exit(-1);
	}
		
	for(i=0;i<count;i++)
	{
		char* name = filesStruct[i]->d_name;
		
		if(IsNotDirectoryPointer(name))
		{
			files[i] = concat(path,name);
		}
	}
	
	(*fileCount) = i;
}

void GetFilesAfterTimeFromGivenDirectory(char* path,char** files,int* modifiedCount,time_t startTime,char** log)
{		
	*modifiedCount = 0;
	char* logBuff = "";
	char startBuff[20];
	strftime(startBuff,20,"%Y-%m-%d %H:%M:%S",GetCurrentTimeStructured());
	logBuff = concat(logBuff,"\nChanhelog recorded on ");
	logBuff = concat(logBuff,startBuff);
	logBuff = concat(logBuff,"\n");

	int count = 0;
	char** filesBuff;
	GetFilesFromGivenDirectory(path,filesBuff,&count);
	printf("Found %d files in directory.\n",count);
	
	for (int i=0;i<count;++i)
	{
		char* pathToCheck = filesBuff[i];
		time_t time = getFileModifiedTime(pathToCheck);
		int diff = difftime(startTime, time);	

		//PrintLastModifiedResults(fileName,pathToCheck,diff);

		if(diff<0)
		{		
			files[(*modifiedCount)] = pathToCheck;
			(*modifiedCount)++;
			
			char buff[20];
			strftime(buff,20,"%Y-%m-%d %H:%M:%S",localtime(&time));
			
			logBuff = concat(logBuff,pathToCheck);
			logBuff = concat(logBuff," \n\tLast modified on ");
			logBuff = concat(logBuff,buff);
			logBuff = concat(logBuff," by <TODO>\n");
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

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void CopyFiles(char* files[],int count,char* srcPath,char* destPath)
{
	char* destPaths[count];
	
	for(int i=0;i<count;i++)
	{
		destPaths[i] = concat(destPath,LastIndexSubStr(files[i],'/'));
	}
	
	for(int i=0;i<count;i++)
	{
		char ch;
		FILE* source = fopen(files[i], "r");
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

char* LastIndexSubStr(char* stringToSearch,char charToGetLastOf)
{
	int index = -1;
	int i;
	for(i=0;stringToSearch[i]!='\0';i++)
	{
		if(stringToSearch[i]==charToGetLastOf)
		{
			index=i;
		}
	}
	
	if(index == -1)
	{
		printf("Character not found in string.");
		exit(-1);
	}

	int length = (i-index)+1;
	char *output = malloc(length);
	memcpy(output, &stringToSearch[i], length-1 );
	output[length] = '\0';
	
	return output;
}
