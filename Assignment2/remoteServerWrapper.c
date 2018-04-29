#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "recentFilesChangedWrapper.h"
#include "remoteServerWrapper.h"
#include "timeWrapper.h"

char* Trim(char* str);
void LogUserUpload(char* username,char* fileModified);

int MAXFILENAMELENGTH = 200;
int MAXFILEPATHLENGTH = 1000;
int MAXFILESIZE = 1000000;
int MAX_USERNAME_LENGTH = 50;
int MAX_PASSWORD_LENGTH = 50;
char* usersFileLocation = "/home/simon/Documents/Assignment2/usersFile";
char* uploadLogFileLocation = "/var/www/html/uploadLog.log";

int RunRemoteServer()
{
	printf("Remote server started. Loading users file...\n");
	
	int connSize = sizeof(struct sockaddr_in); //problem?
	struct sockaddr_in server, client;
	int s = socket(AF_INET,SOCK_STREAM,0);
	
	if(s==-1)
	{
		printf("Error: Could not create socket!\n");
		return -1;
	}
	else
	{
		printf("Socket created.\n");
	}
	
	server.sin_port = htons(8081); 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	
	int result = bind(s,(struct sockaddr *)&server,sizeof(server));
	
	if(result<0)
	{
		printf("Binding error: %d\n",result);
		return -1;
	}
	else
	{
		printf("Bind complete!\n");
	}
	
	listen(s,3);
	printf("Waiting for incoming connection from client.\n");
	
	while(1)
	{
		int newClientSocket = accept(s,(struct sockaddr *)&client,(socklen_t*)&connSize);
		int pid = fork();
		
		if (pid<0) //Error
		{
			printf("Fork error code: %d\n",pid);
			exit(1);
		}
		else if (pid==0) //New process to handle client
		{
			close(s);
			HandleSingleClient(newClientSocket);
			exit(0);
		}
		else //Original process, accepts next client
		{
			close(newClientSocket);
		}
	}
}

int HandleSingleClient(int clientSocket)
{
	int numberOfUsers = 0;
	struct UserAccount* accounts = GetUsernamesAndPasswords(&numberOfUsers);
	char* loggedInUser;

	char fileName[MAXFILENAMELENGTH];
	char filePath[MAXFILEPATHLENGTH];
	char fileData[MAXFILESIZE];
	
	char userName[MAX_USERNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];

	if(clientSocket < 0)
	{
		printf("Can't establish connection!\n");
		return -1;
	}
	else
	{
		printf("Connection from client accepted\n");
	}
	
	while(1)
	{
		int userFound = 0;
		
		memset(userName,0,MAX_USERNAME_LENGTH);
		memset(password,0,MAX_PASSWORD_LENGTH);

		int readSizeUN = recv(clientSocket,userName,MAXFILEPATHLENGTH,0);
		printf("Read username. Size is %d\n",readSizeUN);
		SendDummyMessage(clientSocket);

		int readSizePW = recv(clientSocket,password,MAXFILESIZE,0);
		printf("Read password. Size is %d\n",readSizePW);
		SendDummyMessage(clientSocket);
		
		printf("Checking user %s and password %s\n",userName,password);

		for(int i = 0;i<numberOfUsers;i++)
		{
			printf("Checking against \"%s\" and password \"%s\"\n",accounts[i].username,accounts[i].password);
			
			if(strcmp(Trim(accounts[i].username),Trim(userName)) == 0 && strcmp(Trim(accounts[i].password),Trim(password)) == 0)
			{
				write(clientSocket,"authenticated",strlen("authenticated"));
				printf("User %s authenticated.\n",userName);
				loggedInUser = accounts[i].username;
				userFound = 1;
			}
		}

		if(userFound)
		{
			break;
		}
		else
		{
			write(clientSocket,"rejected",strlen("rejected"));
			printf("User rejected.\n",userName);
		}
	}
	
	/*int errorCode = setsid();
	if(errorCode < 0)
	{
		printf("Error: Could not elevate. Error code %d\n",errorCode);
		exit(-1);
	}*/
	
	while(1)
	{
		int wasFailure = 1;

		memset(fileName,0,MAXFILENAMELENGTH);
		memset(filePath,0,MAXFILEPATHLENGTH);
		memset(fileData,0,MAXFILESIZE);

		int readSize1 = recv(clientSocket,fileName,MAXFILENAMELENGTH,0);
		printf("Read file name. Size is %d\n",readSize1);
		
		if(strcmp(fileName,"Exit")==0)
		{
			break;
		}
		
		sleep(1);
		SendDummyMessage(clientSocket);

		int readSize2 = recv(clientSocket,filePath,MAXFILEPATHLENGTH,0);
		printf("Read file path. Size is %d\n",readSize2);
		sleep(1);
		SendDummyMessage(clientSocket);

		int readSize3 = recv(clientSocket,fileData,MAXFILESIZE,0);
		printf("Read file data. Size is %d\n",readSize3);
		sleep(1);
		SendDummyMessage(clientSocket);
		
		if(!DoesFileHaveEnd(fileData))
		{
			printf("Error: a file was sent which is too long to read.\n");
		}
		else if(readSize1 == 0 ||readSize2 == 0 ||readSize3 == 0)
		{
			printf("Client did not send three messages\n");
			fflush(stdout);
		}
		else if(readSize1 == -1 ||readSize2 == -1 ||readSize3 == -1)
		{
			printf("Read error.\n");
		}
		else
		{
			int errorCode = WriteFile(fileName,filePath,fileData);
			if(errorCode==0)
			{
				wasFailure = 0;
			}
		}
		
		if(!wasFailure)
		{
			write(clientSocket,"Success",strlen("Success"));
			LogUserUpload(loggedInUser,concat(filePath,fileName));
		}
		else
		{
			write(clientSocket,"Failure",strlen("Failure"));
		}
	}
	
	printf("Recieved shutdown message.\n");
	//free(accounts);
	close(clientSocket);
}

int DoesFileHaveEnd(char* fileData)
{
	for(int i=0;i<MAXFILESIZE;i++)
	{
		if(fileData[i]==0)
		{
			return 1;
		}
	}
	
	return 0;
}

int SendDummyMessage(int socketID)
{
	char dummyMessage[500];
	memset(dummyMessage,0,500);
	int errorCode = send(socketID,dummyMessage,500,0);
		
	if(errorCode<0)
	{
		printf("Send failed. Error code:%d\n",errorCode);
		exit(-1);
	}
}

int WriteFile(char* fileName,char* filePath,char* fileData)
{
	if(access(filePath, W_OK) != 0)
	{
		printf("Permission to write to path \"%s\" denied.\n",filePath);
		return -1;
	}

	char* fileWithPath = concat(filePath,fileName);
	printf("Filepath to write: \"%s\"\n",fileWithPath);
	
	
	FILE *fp = fopen(fileWithPath,"w+");
	
	if(fp<0)
	{
		printf("File write error code: %d\n",fp);
		return -1;
	}
	
	printf("File successfully opened for writing.\n");

	for(int i=0;fileData[i]!=0;i++)
	{
		printf("Before %d\n",i);
		fputc(fileData[i],fp);
		printf("After %d\n",i);
	}
	
	printf("Done writing file.\n");

	fclose (fp);
	//should free(fileWithPath) here?
	return 0;
}

struct UserAccount* GetUsernamesAndPasswords(int* length)
{
	printf("Opening file at location %s.\n",usersFileLocation);

	FILE *fp = fopen(usersFileLocation,"r");

	int ch=0;
	int lines=0;

	while(!feof(fp))
	{
		ch = fgetc(fp);
		if(ch == '\n')
		{
			lines++;
		}
	}
	
	printf("File opened and has %d lines.\n",lines);

	
	struct UserAccount* accounts = malloc(lines*sizeof(struct UserAccount));
	
	int i = 0;
	size_t len = 0;
	char* line;
	ssize_t read;
	fseek(fp, 0, SEEK_SET);
	
	while ((read = getline(&line, &len, fp)) != -1) 
	{
		printf("Retrieved line of length %zu :\n", read);
		printf("%s\n", line);
		
		int tokenCount = 0;
		char** tokens = StringSplit(line, ",",&tokenCount);
		
		if(tokenCount != 2)
		{
			printf("Error: Token count (%d) does not equal 2.\n",tokenCount);
		}
		else
		{
			struct UserAccount account;
			account.username = tokens[0];
			account.password = tokens[1];
			accounts[i] = account;
			printf("Read in username \"%s\" and password \"%s\"\n",tokens[0],tokens[1]);
		}
		
		i++;
	}
	
	*(length) = lines;
	return accounts;
}

char* Trim(char* str)
{
  char* end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

void LogUserUpload(char* username,char* fileModified)
{
	char timeString[20];
	strftime(timeString,20,"%Y-%m-%d %H:%M:%S",GetCurrentTimeStructured());

	char *s;
	sprintf(s, "User %s uploaded file %s at %s\n",username,fileModified,timeString);

	FILE* fp;
	fp = fopen(uploadLogFileLocation,"a");
	fprintf(fp,s);
	fclose(fp);
}
