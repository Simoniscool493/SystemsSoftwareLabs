#ifndef RECENTFILESCHANGEDWRAPPER_H
#define RECENTFILESCHANGEDWRAPPER_H

#include <time.h>

int file_select();
time_t getFileModifiedTime();
extern int alphasort();
char** GetFilesFromGivenDirectory(char* path,int* fileCount);
char** GetModifiedFilesFromIntraToAddToLiveSite(char* intraDir,char* liveDir,int* modifiedCount,char** log);
int GetFileCountFromGivenDirectory(char* path);
void PrintStringArray(char* stringArray[],int length);
void CopyFiles(char* files[],int count,char* srcPath,char* destPath);
void CreateDirectoriesIfTheyDoNotExist(char* relativePath,char* rootPath);
void LockAllFilesInDirectory(char* path);
void FreeStringArray(char** stringArray,int count);

#endif



