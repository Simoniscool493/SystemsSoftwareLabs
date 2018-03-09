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
	printf("PrintStringArray.\n");

	for(int i=0;i<length;i++)
	{
		printf("%s\n",stringArray[i]);
	}
}

char** GetFilesFromGivenDirectory(char* path,int* fileCount)
{
	printf("GetFilesFromGivenDirectory.\n");

	int count,i;
	struct direct **filesStruct;
	count = scandir(path, &filesStruct, file_select, alphasort);
	
	char ** files = malloc(count * sizeof(char*));
	
	if(count<0)
	{
		printf("Scandir error code: %d\n",count);
		exit(-1);
	}
	
	int j = 0;
		
	for(i=0;i<count;i++)
	{
		char* name = filesStruct[i]->d_name;
		
		if(IsNotDirectoryPointer(name))
		{
			printf("files[%d] is %s\n",i,name);
			files[j] = concat(path,name);
			j++;
		}
	}
	
	printf("Filecount is %d\n",j);
	printf("In getFiles, first file is %s\n",files[0]);
	(*fileCount) = j;
	return files;
}

char** GetFilesAfterTimeFromGivenDirectory(char* path,int* modifiedCount,time_t startTime,char** log)
{		
	printf("GetFilesAfterTimeFromGivenDirectory.\n");

	*modifiedCount = 0;
	char* logBuff = "";
	char startBuff[20];
	strftime(startBuff,20,"%Y-%m-%d %H:%M:%S",GetCurrentTimeStructured());
	logBuff = concat(logBuff,"\nChanhelog recorded on ");
	logBuff = concat(logBuff,startBuff);
	logBuff = concat(logBuff,"\n");

	int count = 0;
	char** filesBuff = GetFilesFromGivenDirectory(path,&count);
	printf("Found %d files in directory.\n",count);
	
	char ** files = malloc(count * sizeof(char*));
	
	for (int i=0;i<count;++i)
	{
		char* pathToCheck = filesBuff[i];
		time_t time = getFileModifiedTime(pathToCheck);
		int diff = difftime(startTime, time);	

		//PrintLastModifiedResults(fileName,pathToCheck,diff);

		if(diff<0)
		{		
			files[(*modifiedCount)] = pathToCheck;
			*(modifiedCount)++;
			
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
	return files;
	//printf("Created log message: %s",log);
}

int IsNotDirectoryPointer(char* fileName)
{
	printf("IsNotDirectoryPointer.\n");

	if(strcmp(".",fileName)!=0 && strcmp("..",fileName)!=0)
	{
		return 1;
	}
	
	return 0;
}

void PrintLastModifiedResults(char* fileName,char* filePath,int seconds)
{
	printf("PrintLastModifiedResults.\n");

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
	printf("GetFileCountFromGivenDirectory.\n");

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
	printf("getFileModifiedTime.\n");

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
	printf("concat.\n");

    char *result = malloc(strlen(s1)+strlen(s2)+1);

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void CopyFiles(char* files[],int count,char* srcPath,char* destPath)
{
	printf("CopyFiles.\n");

	char* destPaths[count];
	
	for(int i=0;i<count;i++)
	{
		printf("file path to concat is %s\n",files[i]);
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
	printf("LastIndexSubStr.\n");
	printf("String to search is %s\n",stringToSearch);
	printf("Char to find is %c\n",charToGetLastOf);

	int index = -1;
	int i;
	for(i=0;stringToSearch[i]!='\0';i++)
	{
		if(stringToSearch[i]==charToGetLastOf)
		{
			index=i;
		}
	}
	
	printf("i is %d\n",i);
	printf("index is %d\n",index);

	if(index == -1)
	{
		printf("Character not found in string.");
		exit(-1);
	}

	int length = (i-index);
	
	printf("length is %d\n",length);
	char *output = malloc(length);
	memcpy(output, stringToSearch+index+1, length);
	output[length] = '\0';
	
	printf("output is %s\n",output);
	
	return output;
}

