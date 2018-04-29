#include <stdio.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/file.h>
#include <poll.h>
#include <syslog.h>
#include <string.h>

#include "fifoWrapper.h"
#include "recentFilesChangedWrapper.h"
#include "timeWrapper.h"
#include "remoteServerWrapper.h"

void BackupLiveSite();
void UpdateLiveSite();
void FetchIntraSiteChangeLog();

int IsTimeToBackup();
void BackupAndUpdateIfTimeTo();
void FetchIntraSiteChangeLogIfTimeTo();

void ForkFifoProcess();
void SetToDaemon();
void Log(char* message);

time_t mostRecentChangelogTime;
time_t nextTimeToBackup;

int changeLogIntervalInSeconds = 9999;
int hourToBackUp = 23;

char* liveDir = "/var/www/html/liveSite/";	
char* backupDir = "/var/www/html/backup/";
char* intraDir = "/var/www/html/intra/";
char* updateLogFileLocation = "/var/www/html/updateLog.log";
char* changeLogFileLocation = "/var/www/html/changeLog.log";
char* auditLogFleLocation = "/var/log/audit/audit.log";
char* watchedDirectory = "var/www/html";

int main()
{		
	openlog("SystemsSoftwareAssignmentProgramServer", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	
	ForkFifoProcess();
	nextTimeToBackup = GetTimeAtRelativeDay(0,hourToBackUp,0);
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
		
		printf("Locking live directory...\n");
		//LockAllFilesInDirectory(liveDir);
		printf("Live directory locked.\n");
		
		pid = fork();
		
		if(pid==0)
		{
			printf("Starting remote server thread.\n");
			RunRemoteServer();
			return 0;
		}
		else if(pid<0)
		{
			printf("Fork error when creating remote server thread.\n");
		}
		
		printf("Starting main thread.\n");	

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
					FetchIntraSiteChangeLog();
				}	
				else if(data==3)
				{
					printf("Manual Backup triggered.\n");
					BackupLiveSite();
				}				
				else if(data==4)
				{
					printf("Manual Update triggered.\n");
					UpdateLiveSite();
				}				
				else if(data==5)
				{
					printf("Recieved shutdown message.\n");
					break;
				}
			}
			
			BackupAndUpdateIfTimeTo();
			FetchIntraSiteChangeLogIfTimeTo();

			sleep(1);
		}
	}
	else
	{
		printf("Fork error when forking to daemon.\n");
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
		nextTimeToBackup = GetTimeAtRelativeDay(1,hourToBackUp,0);
	}
}

void FetchIntraSiteChangeLogIfTimeTo()
{
	int secsPassed = difftime(GetCurrentTimeRaw(),mostRecentChangelogTime);
	
	if(secsPassed>changeLogIntervalInSeconds)
	{
		printf("%d seconds have passed. Starting scheduled changelog.\n",changeLogIntervalInSeconds);
		FetchIntraSiteChangeLog();
	}
}

void UpdateLiveSite()
{
	int count;
	char* changeLog;
	char** files = GetModifiedFilesFromIntraToAddToLiveSite(intraDir,liveDir,&count,&changeLog);

	//PrintStringArray(files,count);
	
	if(count>0)
	{		
		CopyFiles(files,count,intraDir,liveDir);
		Log(changeLog);
	}
	
	FreeStringArray(files,count);
}

void FetchIntraSiteChangeLog()
{	
	FILE * auditFile = fopen(auditLogFleLocation, "r");
	printf("Audit file opened, value of %d\n",auditFile);
	
	FILE * changeLog = fopen(changeLogFileLocation, "w");
	printf("Changelog file opened, value of %d\n",changeLog);

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	if (auditFile < 0)
	{
		printf("File error code: %d\n",auditFile);
		exit(EXIT_FAILURE);
	}
	
	int count = 0;
	printf("Fetching and writing data...\n");

	while ((read = getline(&line, &len, auditFile)) != -1) 
	{
		if(
			(strstr(line,watchedDirectory) != NULL) /*&&
		    (strstr(line," auid=") != NULL)*/
		)
		{
			fprintf(changeLog,line);
			count++;
		}
	}

	printf("Finiahed writing data.\n");

	fclose(auditFile);
	fclose(changeLog);

	if (line)
	{
		free(line);
	}
	
	printf("Total count: %d\n",count);

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
	FreeStringArray(files,count);
}

void SetToDaemon()
{
	printf("SetToDaemon.\n");

	int errorCode = setsid();
	if(errorCode < 0)
	{
		syslog(LOG_DAEMON, "Daemon Error: Could not elevate. Error code %d\n",errorCode);
		printf("Daemon Error: Could not elevate. Error code %d\n",errorCode);
		exit(EXIT_FAILURE);
	}

	umask(0);
	
	errorCode = chdir("/");
	if(errorCode < 0)
	{
		syslog(LOG_DAEMON, "Daemon Error: Could not set wd to root. Error code %d\n",errorCode);
		printf("Daemon Error: Could not set wd to root. Error code %d\n",errorCode);
		exit(EXIT_FAILURE);
	}

	errorCode = setuid(geteuid());
	if(errorCode < 0)
	{
		syslog(LOG_DAEMON, "Daemon Error: Could not set uid to root. Error code %d\n",errorCode);
		printf("Daemon Error: Could not set uid to root. Error code %d\n",errorCode);
		exit(EXIT_FAILURE);
	}
	
}

void Log(char* message)
{
	printf("Log.\n");

	FILE* fp;
	fp = fopen(updateLogFileLocation,"a");
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
	//printf("IsTimeToBackup.\n");
	
	if(IsTimeAfter(GetCurrentTimeRaw(),nextTimeToBackup))
	{
		return 1;
	}
	
	return 0;
}



