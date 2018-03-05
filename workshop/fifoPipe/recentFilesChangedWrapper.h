#ifndef RECENTFILESCHANGEDWRAPPER_H
#define RECENTFILESCHANGEDWRAPPER_H

#include <time.h>

int file_select();
time_t getFileModifiedTime();
extern int alphasort();
void GetFilesAfterTimeFromGivenDirectory(char* path,char* modifiedFiles[] ,int* modifiedCount,time_t startTime,char** log,int ignoreTime);
time_t GetCurrentTimeRaw();
struct tm* GetCurrentTimeStructured();
int GetFileCountFromGivenDirectory(char* path);
void PrintStringArray(char* stringArray[],int length);
void CopyFiles(char* files[],int count,char* srcPath,char* destPath);

#endif
