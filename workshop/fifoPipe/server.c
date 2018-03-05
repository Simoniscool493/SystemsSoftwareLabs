#include<stdio.h> 
#include<fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/file.h>
#include <poll.h>

#include "fifoWrapper.h"
#include "recentFilesChangedWrapper.h"

void BackupAndUpdate();
void LogChangesToIntraSite(int shouldCopyFiles);

void BackupAndUpdateIfTimeTo();
void ChangeLogIfTimeTo();

void ForkFifoProcess();
void SetToDaemon();
void Log(char* message);

time_t mostRecentBackupTime;
time_t mostRecentChangelogTime;
time_t mostRecentLiveSiteUpdateTime;

int backupIntervalInSeconds = 999;
int changeLogIntervalInSeconds = 999;
int liveSiteUpdateIntervalInSeconds = 999;

char* liveDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/liveSite/";	
char* backupDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/backup/";
char* intraDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/intra/";
char* logFileLocation = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/changelog.log";

int main()
{	
	ForkFifoProcess();
	mostRecentBackupTime = GetCurrentTimeRaw();
	mostRecentChangelogTime = GetCurrentTimeRaw();
	mostRecentLiveSiteUpdateTime = GetCurrentTimeRaw();

	int fifo_server = OpenFIFO("fifo_server");
	int pid = fork();

	if(pid > 0)
	{
		sleep(2);
		exit(EXIT_SUCCESS);
	}
	else if(pid ==0)
	{
		SetToDaemon();
		
		while(1)
		{			
			if (poll(&(struct pollfd){ .fd = fifo_server, .events = POLLIN }, 1, 0)==1) 
			{
				int data = ReadFIFO(fifo_server);
				printf("External data recieved: %d\n",data);
				
				if(data==1)
				{
					printf("Manual Backup and update triggered.\n");
					BackupAndUpdate();
				}
				else if(data==2)
				{
					printf("Manual Changelog triggered.\n");
					LogChangesToIntraSite(0);
				}				
				else if(data==3)
				{
					printf("Recieved shutdown message.\n");
					break;
				}

			}
			
			BackupAndUpdateIfTimeTo();
			ChangeLogIfTimeTo();

			sleep(1);
		}
	}
	
	printf("Shutting down...\n");
	close(fifo_server);
	
	return 0;
}

void ForkFifoProcess()
{
	pid_t pid=fork();
	
	if (pid==0) 
	{
		CreateFIFO("fifo_server"); 
		exit(0);
	}
	else 
	{ 
		waitpid(pid,0,0); 
	}
}

void BackupAndUpdateIfTimeTo()
{
	int secsPassed = difftime(GetCurrentTimeRaw(),mostRecentBackupTime);
	
	if(secsPassed>backupIntervalInSeconds)
	{
		printf("%d seconds have passed. Starting scheduled backup.\n",backupIntervalInSeconds);
		BackupAndUpdate();
	}
}

void ChangeLogIfTimeTo()
{
	int secsPassed = difftime(GetCurrentTimeRaw(),mostRecentChangelogTime);
	
	if(secsPassed>changeLogIntervalInSeconds)
	{
		printf("%d seconds have passed. Starting scheduled changelog.\n",changeLogIntervalInSeconds);
		LogChangesToIntraSite(0);
	}
}

void UpdateLiveSite()
{
	LogChangesToIntraSite(1);
}

void LogChangesToIntraSite(int shouldCopyFiles)
{	
	int count;
	char* files[GetFileCountFromGivenDirectory(intraDir)];
	char* changeLog;
	GetFilesAfterTimeFromGivenDirectory(intraDir,files,&count,mostRecentChangelogTime,&changeLog,0);
	
	//PrintStringArray(files,count);
	
	if(count>0)
	{		
		if(shouldCopyFiles)
		{
			CopyFiles(files,count,intraDir,liveDir);
		}
		
		Log(changeLog);
	}
	
	mostRecentChangelogTime = GetCurrentTimeRaw();
}

void BackupAndUpdate()
{	
	//flock(sourceDir, LOCK_EX);
	//printf("Directory locked. Sleeping for 20...\n");
	//sleep(20);

	int count;
	char* files[GetFileCountFromGivenDirectory(liveDir)];
	char* log;
	GetFilesAfterTimeFromGivenDirectory(liveDir,files,&count,mostRecentBackupTime,&log,1);
	//PrintStringArray(files,count);
	
	CopyFiles(files,count,liveDir,backupDir);
	
	mostRecentBackupTime = GetCurrentTimeRaw();
	//flock(sourceDir, LOCK_UN);
	UpdateLiveSite();
}

void SetToDaemon()
{
	int errorCode = setsid();
	if(errorCode < 0)
	{
		printf("Daemon Error: Could not elevate. Error code %d\n",errorCode);
		exit(EXIT_FAILURE);
	}

	umask(0);
	
	errorCode = chdir("/");
	if(errorCode < 0)
	{
		printf("Daemon Error: Could not set wd to root. Error code %d\n",errorCode);
		exit(EXIT_FAILURE);
	}
}

void Log(char* message)
{
	FILE* fp;
	fp = fopen(logFileLocation,"a");
	fprintf(fp,message);
	fclose(fp);
}
