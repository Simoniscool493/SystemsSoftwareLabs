#include <stdio.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/file.h>
#include <poll.h>

#include "fifoWrapper.h"
#include "recentFilesChangedWrapper.h"
#include "timeWrapper.h"

void BackupLiveSite();
void UpdateLiveSite();
void LogChangesToIntraSite(int shouldCopyFiles);

int IsTimeToBackup();
void BackupAndUpdateIfTimeTo();
void LogChangesToIntraSiteIfTimeTo();

void ForkFifoProcess();
void SetToDaemon();
void Log(char* message);

time_t mostRecentChangelogTime;
time_t nextTimeToBackup;

int changeLogIntervalInSeconds = 2;

char* liveDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/liveSite/";	
char* backupDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/backup/";
char* intraDir = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/intra/";
char* logFileLocation = "/home/simon/Documents/SystemsSoftwareLabs/workshop/fifoPipe/website/changelog.log";

int main()
{	
	ForkFifoProcess();
	nextTimeToBackup = GetTimeAtRelativeDay(0,10,0,0);
	mostRecentChangelogTime = GetCurrentTimeRaw();

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
					BackupLiveSite();
					UpdateLiveSite();
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
			LogChangesToIntraSiteIfTimeTo();

			sleep(1);
		}
	}
	
	printf("Shutting down...\n");
	close(fifo_server);
	
	return 0;
}

void BackupAndUpdateIfTimeTo()
{
	if(IsTimeToBackup())
	{
		printf("Starting scheduled backup and update of live site.\n");
		BackupLiveSite();
		UpdateLiveSite();
		nextTimeToBackup = GetTimeAtRelativeDay(1,10,0,0);
	}
}

void LogChangesToIntraSiteIfTimeTo()
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
	char* changeLog;
	char** files = GetFilesAfterTimeFromGivenDirectory(intraDir,&count,mostRecentChangelogTime,&changeLog);

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

void BackupLiveSite()
{	
	printf("BackupLiveSite.\n");
	//flock(sourceDir, LOCK_EX);
	//printf("Directory locked. Sleeping for 20...\n");
	//sleep(20);

	int count;
	char** files = GetFilesFromGivenDirectory(liveDir,&count);
	
	printf("in backup, first file is %s\n",files[0]);

	//PrintStringArray(files,count);
	
	CopyFiles(files,count,liveDir,backupDir);
	
	//flock(sourceDir, LOCK_UN);
}

void SetToDaemon()
{
	printf("SetToDaemon.\n");

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
	printf("Log.\n");

	FILE* fp;
	fp = fopen(logFileLocation,"a");
	fprintf(fp,message);
	fclose(fp);
}

void ForkFifoProcess()
{
	printf("ForkFifoProcess.\n");

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

int IsTimeToBackup()
{
	printf("IsTimeToBackup.\n");
	
	if(IsTimeAfter(GetCurrentTimeRaw(),nextTimeToBackup))
	{
		return 1;
	}
	
	return 0;
}

