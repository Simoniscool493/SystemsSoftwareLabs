#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>

#include "recentFilesChangedWrapper.h"
#include "timeWrapper.h"

char* concat(const char *s1, const char *s2);
int IsNotDirectoryPointer(char* fileName);
void PrintLastModifiedResults(char* fileName,char* filePath,int seconds);
char* LastIndexSubStr(char* stringToSearch,char charToGetLastOf);
char* RemoveMainPath(char* path,char* mainPath);
char* GetPathFromPathListContaningThisRelativePath(char* path,char* directoryPath,char** list,int listLength,int* wasFound);
int IsFileNewerThan(char* path1,char* path2);
char** CombineStringArrays(char** s_one,int sizeOfFirstArray,char** s_two,int sizeOfSecondArray);
int IsPathDirectory(char *path);
char** StringSplit(char* string, const char* delimiter,int* totalCount);
int FileExists(char *path);


void PrintStringArray(char* stringArray[],int length)
{
	printf("PrintStringArray.\n");

	for(int i=0;i<length;i++)
	{
		printf("\t%s\n",stringArray[i]);
	}
	
	printf("Done print string array!\n");
}

char** GetFilesFromGivenDirectory(char* path,int* fileCount)
{
	printf("GetFilesFromGivenDirectory.\n");

	int count,i;
	struct direct **filesStruct;
	count = scandir(path, &filesStruct, file_select, alphasort);
	
	char ** files = malloc((count+1) * sizeof(char*));
	char ** dirs = malloc((count+1) * sizeof(char*));

	if(count<0)
	{
		printf("Scandir error code: %d\n",count);
		exit(-1);
	}
	
	int numFiles = 0;
	int numDirs = 0;

	for(i=0;i<count;i++)
	{
		char* name = filesStruct[i]->d_name;
		
		if(IsNotDirectoryPointer(name))
		{
			printf("files[%d] is %s\n",i,name);
			char* fullPath = concat(path,name);

			if(IsPathDirectory(fullPath))
			{
				dirs[numDirs] = concat(fullPath,"/");
				numDirs++;
			}
			else
			{
				files[numFiles] = fullPath;
				numFiles++;
			}
		}
	}
	
	for(i=0;i<numDirs;i++)
	{
		printf("printing original string array\n");
		PrintStringArray(files,numFiles);
		
		int curCount = 0;
		char** folderContents = GetFilesFromGivenDirectory(dirs[i],&curCount);
		
		printf("Folder contents is as follows:\n");
		PrintStringArray(folderContents,curCount);

		files = CombineStringArrays(files,numFiles,folderContents,curCount);
		

		numFiles+=curCount;
		
		printf("printing combined string array\n");
		PrintStringArray(files,numFiles);
	}
	
	printf("Filecount is %d\n",numFiles);
	
	if(numFiles>0)
	{
		printf("In getFiles, first file is %s\n",files[0]);
	}
	else
	{
		printf("In getFiles, first file was not found because there aren't any.");
	}
	
	printf("Freeing dirs...\n");
	
	FreeStringArray(dirs,numDirs);
	printf("Freed dirs.\n");
	
	(*fileCount) = numFiles;
	return files;
}

void FreeStringArray(char** stringArray,int count)
{
	for(int i=0;i<count;i++)
	{
		free(stringArray[i]);
	}
	
	free(stringArray);
}

char** GetModifiedFilesFromIntraToAddToLiveSite(char* intraDir,char* liveDir,int* modifiedCount,char** log)
{		
	printf("GetFilesAfterTimeFromGivenDirectory.\n");

	*modifiedCount = 0;
	char* logBuff = "";
	char startBuff[20];
	strftime(startBuff,20,"%Y-%m-%d %H:%M:%S",GetCurrentTimeStructured());
	logBuff = concat(logBuff,"\nChangelog recorded on ");
	logBuff = concat(logBuff,startBuff);
	logBuff = concat(logBuff,"\n");

	int countIntra = 0;
	char** intraFiles = GetFilesFromGivenDirectory(intraDir,&countIntra);
	printf("Found %d files in intra directory.\n",countIntra);
	
	int countLive = 0;
	char** liveFiles = GetFilesFromGivenDirectory(liveDir,&countLive);
	printf("Found %d files in live directory.\n",countLive);
	
	char ** modifiedFiles = malloc((countIntra+1) * sizeof(char*));
	printf("allocated modified files\n");
	
	for (int i=0;i<countIntra;i++)
	{
		printf("intra dir new file check #%d\n",i);

		char* intraPathToCheck = intraFiles[i];
		printf("intraPathToCheck assigned\n");

		char* intraRelativePath = RemoveMainPath(intraPathToCheck,intraDir);
				printf("intraRelativePath assigned\n");

		int wasFound = 0;
		char* livePathToCheck = GetPathFromPathListContaningThisRelativePath(intraRelativePath,liveDir,liveFiles,countLive,&wasFound);
						printf("livePathToCheck assigned\n");

		
		if((!wasFound) || IsFileNewerThan(intraPathToCheck,livePathToCheck))
		{
									printf("IsFileNewerThan called\n");

			modifiedFiles[(*modifiedCount)] = strdup(intraPathToCheck);
			printf("Adding intra path: %s.\n",intraPathToCheck);

			(*modifiedCount)++;
			
			time_t fileModTime = getFileModifiedTime(intraPathToCheck);
			char buff[20];
 			strftime(buff,20,"%Y-%m-%d %H:%M:%S",localtime(&fileModTime));
			char* logMessage = concat(intraPathToCheck,concat("\n\tEdited on ",buff));
			logBuff = concat(logBuff,logMessage);
		}
	}
	
	printf("First file is %s. (before freeing)\n",modifiedFiles[0]);

	printf("freeing intrafiles...\n");
	FreeStringArray(intraFiles,countIntra);

	//printf("freeing liveFiles...\n");
	//free(liveFiles);
	
	printf("Found %d modified files in directory. \n",*modifiedCount);
	printf("First file is %s.\n",modifiedFiles[0]);

	logBuff = concat(logBuff,"\n\n");
	*(log) = logBuff;
	return modifiedFiles;
	//printf("Created log message: %s",log);
}

void LockAllFilesInDirectory(char* path)
{
	int count =0;
	char** files = GetFilesFromGivenDirectory(path,&count);
	
	for(int i=0;i<count;i++)
	{
		chmod(files[i],0444);
	}
	
	FreeStringArray(files,count);
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
		syslog(LOG_DAEMON, "Scandir error code: %d",count);
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
		syslog(LOG_DAEMON, "stat() error code: %d",errorCode);
		printf("stat() error code: %d",errorCode);
	}

	time_t time = attr.st_mtime;

	return time;
}

int IsPathDirectory(char *path) 
{
	printf("IsPathDirectory.\n");
	printf("path is %s.\n",path);

    struct stat attr;
    stat(path, &attr);
	int ans = S_ISDIR(attr.st_mode);
	
	if(ans)
	{
		printf("path IS dir.\n");
	}
	else
	{
		printf("path is NOT dir.\n");
	}
    return ans;
}

char* concat(const char *s1, const char *s2)
{
	//printf("concat.\n");

    char *result = malloc(strlen(s1)+strlen(s2)+1);

    strcpy(result, s1);
    strcat(result, s2);
	//printf("end concat.\n");

    return result;
}

void CopyFiles(char* files[],int count,char* srcPath,char* destPath)
{
	printf("CopyFiles.\n");

	char* destPaths[count];
	
	printf("1# copyfiles count is %d\n",count);

	for(int i=0;i<count;i++)
	{
		printf("1# file #%d (path %s)\n",i,files[i]);
		destPaths[i] = concat(destPath,RemoveMainPath(files[i],srcPath));
		printf("1# done file %d\n",i);
	}
	
	printf("Got all destpaths\n");
	printf("2# copyfiles count is %d\n",count);

	for(int i=0;i<count;i++)
	{
		printf("2# file #%d (path %s)\n",i,files[i]);
		CreateDirectoriesIfTheyDoNotExist(RemoveMainPath(files[i],srcPath),destPath);

		char ch;
		FILE* source = fopen(files[i], "r");
					printf("fopened source.\n");

		remove(destPaths[i]);
							printf("removed.\n");

		FILE* dest = fopen(destPaths[i], "w");
							printf("2# fopened dest. (%s)\n",destPaths[i]);

		
		while(( ch = fgetc(source)) != EOF )
		{
			fputc(ch, dest);
		}
							printf("written.\n");


		fclose(source);
		fclose(dest);
	}	
}

void CreateDirectoriesIfTheyDoNotExist(char* relativePath,char* rootPath)
{
	printf("CreateDirectoriesIfTheyDoNotExist\n");
	
	char* totalPath = concat(rootPath,relativePath);
	printf("Total path is %s\n",totalPath);
	
	if(FileExists(totalPath))
	{
		return;
	}
	
	int count = 0;
	char** tokens = StringSplit(relativePath,"/",&count);
	char *curPath = strdup (rootPath);
	
	//printf("Printing tokens from Split():\n");
	//PrintStringArray(tokens,count);

	for(int i=0;i<count-1;i++)
	{
		//printf("Old Curpath is %s\n",curPath);
		//printf("token is %s\n",tokens[i]);

		curPath = concat(curPath,concat(tokens[i],"/"));
		//printf("New Curpath is %s\n",curPath);

		if(!FileExists(curPath))
		{
			mkdir(curPath,0777);
		}
	}
	
	FreeStringArray(tokens,count);
	free(curPath);
}

int FileExists(char *path)
{
	struct stat buffer;   
	return(stat(path, &buffer) == 0);
}	

char* RemoveMainPath(char* path,char* mainPath)
{
	//printf("RemoveMainPath\n");
	/*printf("path is %s\n",path);
	printf("mainPath is %s\n",mainPath);*/

	int pathLength = 0;
	for(pathLength=0;path[pathLength]!='\0';pathLength++){};

	int mainPathLength = 0;
	for(mainPathLength=0;mainPath[mainPathLength]!='\0';mainPathLength++){};
	
	/*printf("pathLength is %d\n",pathLength);
	printf("mainPathLength is %d\n",mainPathLength);*/

	int outputLength = pathLength-mainPathLength;

	char* output = malloc(outputLength+1);
	memcpy(output, path+mainPathLength, outputLength);
	output[outputLength] = '\0';
	
	/*printf("Removed path string length is %d\n",outputLength);
	printf("Removed path string is %s\n",output);*/
	//printf("end RemoveMainPath.\n");
	return output;
}

char* GetPathFromPathListContaningThisRelativePath(char* path,char* directoryPath,char** list,int listLength,int* wasFound)
{
	//printf("GetPathFromPathListContaningThisRelativePath.\n");

	for(int i=0;i<listLength;i++)
	{
		char* relativePath = RemoveMainPath(list[i],directoryPath);
		
		if(strcmp(path,relativePath) == 0)
		{
			(*wasFound) = 1;
			return list[i];
		}
		
		free(relativePath);
	}
	
	(*wasFound) = 0;
	return NULL;
}

int IsFileNewerThan(char* path1,char* path2)
{
	//printf("IsFileNewerThan.\n");

	time_t time1 = getFileModifiedTime(path1);
	time_t time2 = getFileModifiedTime(path2);
	
	int isNewer = IsTimeAfter(time1,time2);
	return isNewer;
}

char* LastIndexSubStr(char* stringToSearch,char charToGetLastOf)
{
	/*printf("LastIndexSubStr.\n");
	printf("String to search is %s\n",stringToSearch);
	printf("Char to find is %c\n",charToGetLastOf);*/

	int index = -1;
	int i;
	for(i=0;stringToSearch[i]!='\0';i++)
	{
		if(stringToSearch[i]==charToGetLastOf)
		{
			index=i;
		}
	}
	
	//printf("i is %d\n",i);
	//printf("index is %d\n",index);

	if(index == -1)
	{
		printf("Character not found in string.");
		exit(-1);
	}

	int length = (i-index);
	
	//printf("length is %d\n",length);
	char *output = malloc(length+1);
	memcpy(output, stringToSearch+index+1, length);
	output[length] = '\0';
	
	//printf("output is %s\n",output);
	
	return output;
}

char** CombineStringArrays(char** s_one,int sizeOfFirstArray,char** s_two,int sizeOfSecondArray)
{
    char **output = malloc((sizeOfFirstArray + sizeOfSecondArray)*sizeof(char*));
	
	for(int i=0;i<sizeOfFirstArray;i++)
	{
		output[i] = strdup(s_one[i]);
	}
	
	for(int i=0;i<sizeOfSecondArray;i++)
	{
		output[i+sizeOfFirstArray] = strdup(s_two[i]);
	}

	FreeStringArray(s_one,sizeOfFirstArray);
	FreeStringArray(s_two,sizeOfSecondArray);
	
	printf("sizeOfFirstArray: %d\n", sizeOfFirstArray);
	printf("sizeOfSecondArray: %d\n", sizeOfSecondArray);
	printf("totalSize: %d\n", sizeOfFirstArray+sizeOfSecondArray);
	
	printf("Printing result before returning from CombineStringArrays...\n");
	PrintStringArray(output,sizeOfFirstArray+sizeOfSecondArray);
	
	return output;
}

char** StringSplit(char* string, const char* delim,int* numtokens)
{
    char *s = strdup(string);
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);token != NULL;token = strtok_r(NULL, delim, &strtok_ctx))
	{
        if (tokens_used == tokens_alloc) 
		{
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    // cleanup
    if (tokens_used == 0) 
	{
        free(tokens);
        tokens = NULL;
    } 
	else 
	{
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
	
    *numtokens = ((int)tokens_used);
	
    free(s);
	
    return tokens;
}

