#ifndef RECENTFILESCHANGEDWRAPPER_H
#define RECENTFILESCHANGEDWRAPPER_H

#include <time.h>

int file_select();
time_t getFileModifiedTime();
extern int alphasort();
void GetFilesFromGivenDirectory(char* path,char** files,int* fileCount);
void GetFilesAfterTimeFromGivenDirectory(char* path,char** files,int* modifiedCount,time_t startTime,char** log);
int GetFileCountFromGivenDirectory(char* path);
void PrintStringArray(char* stringArray[],int length);
void CopyFiles(char* files[],int count,char* srcPath,char* destPath);

#endif

