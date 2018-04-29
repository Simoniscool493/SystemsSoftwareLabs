#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

char* ReadInFile(char* fileName,long* length);
int SendToServer(char* messageToSend,int messageLength,int socketId);
int RecieveDummyMessage(int socketID);
char* SelectSiteDirectory(int* length);

int MAX_USERNAME_LENGTH = 50;
int MAX_PASSWORD_LENGTH = 50;

int MAX_FILE_SIZE = 1000000;
int SERVER_MESSAGE_LENGTH = 500;
int MAX_FILEPATH_LENGTH = 1000;
int PORT_NUMBER = 8081;
char* IP_ADDRESS = "127.0.0.1";

int main(int argc,char* argv[])
{
	struct sockaddr_in server;
	
	char userName[MAX_USERNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	
	char fileNameToSend[MAX_FILEPATH_LENGTH];
	char fileNameToSaveAs[MAX_FILEPATH_LENGTH];
	
	char serverMessage[SERVER_MESSAGE_LENGTH];	
	int s = socket(AF_INET,SOCK_STREAM,0);
	
	if(s==-1)
	{
		printf("Error creating socket\n");
		return -1;
	}
	else
	{
		printf("Socket created\n");
	}
	
	server.sin_port = htons(PORT_NUMBER); 
	server.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	server.sin_family = AF_INET;
	
	int errorCode = connect(s,(struct sockaddr *)&server,sizeof(server));
	
	if(errorCode<0)
	{
		printf("Error connecting socket. Error code %d.\n",errorCode);
		return -1;
	}
	
	printf("Connected to server!\n");
	
	while(1)
	{
		memset(serverMessage,0,SERVER_MESSAGE_LENGTH);
		memset(userName,0,MAX_USERNAME_LENGTH);
		memset(password,0,MAX_PASSWORD_LENGTH);

		printf("Enter Username.\n");
		scanf("%s",userName);
			
		printf("Enter password.\n");
		scanf("%s",password);
			
		SendToServer(userName,strlen(userName),s);
		RecieveDummyMessage(s);
		SendToServer(password,strlen(password),s);
		RecieveDummyMessage(s);

		errorCode = recv(s,serverMessage,SERVER_MESSAGE_LENGTH,0);
		
		if(errorCode<0)
		{
			printf("Recieve message from server failed while authenticating. Error code %d\n",errorCode);
			return -1;
		}

		if(strcmp(serverMessage,"authenticated")==0)
		{
			printf("Successfully authenticated\n");
			break;
		}
		
		printf("Username or password was incorrect.\n");
	}

	while(1)
	{
		memset(serverMessage,0,SERVER_MESSAGE_LENGTH);
		
		printf("\nEnter fileName or absolute path of file to transfer:\n");
		scanf("%s",fileNameToSend);
				
		if(strcmp(fileNameToSend,"Exit")==0)
		{
			SendToServer("Exit",sizeof("Exit"),s);
			close(s);
			break;
		}
		
		printf("\nEnter the name to save the file as on the server:\n");
		scanf("%s",fileNameToSaveAs);

		int siteDirectoryPathLength = 0;
		char* siteDirectoryPath = SelectSiteDirectory(&siteDirectoryPathLength);
		
		long fileLength = 0;
		char* fileData = ReadInFile(fileNameToSend,&fileLength);
		
		printf("Finished reading file. Sending data...\n");
		
		SendToServer(fileNameToSaveAs,strlen(fileNameToSaveAs),s);
		RecieveDummyMessage(s);
		SendToServer(siteDirectoryPath,siteDirectoryPathLength,s);
		RecieveDummyMessage(s);
		SendToServer(fileData,fileLength,s);
		RecieveDummyMessage(s);
		
		printf("All data sent.\n");

		errorCode = recv(s,serverMessage,SERVER_MESSAGE_LENGTH,0);
		
		if(errorCode<0)
		{
			printf("Recieve message from server failed while waiting for result. Error code %d\n",errorCode);
			break;
		}
		
		printf("Server sent:\n");
		printf(serverMessage);
	}
	
	close(s);
	return 0;
}

int SendToServer(char* messageToSend,int messageLength,int socketId)
{
	int errorCode = send(socketId,messageToSend,messageLength,0);
		
	if(errorCode<0)
	{
		printf("Send failed. Error code:%d\n",errorCode);
		exit(-1);
	}
	
	printf("Sent data. Message length was %d.\n",messageLength);
}

char* ReadInFile(char* fileName,long* length)
{
	FILE *f;
	f = fopen(fileName,"r");
	
	if(f<0)
	{
		printf("File open error: %d\n",f);
		exit(-1);
	}

	fseek(f, 0, SEEK_END); // seek to end of file
	long size = ftell(f); // get current file pointer
	fseek(f, 0, SEEK_SET); // seek back to beginning of file
	
	if(size + 1 > MAX_FILE_SIZE)
	{
		printf("File is %ld bytes long. Too large to transfer.\n",size);
		exit(-1);
	}
	
	char *fileData = malloc(size + 1);
	fread(fileData, size, 1, f);
	fclose(f);

	fileData[size] = 0;
	(*length) = size+1;
	return fileData;
}

char* SelectSiteDirectory(int* length)
{
	printf("Please select directory to transfer to on the intranet site.\n");
	printf("\t 1: Intranet root directory.\n");
	printf("\t 2: Sales directory.\n");
	printf("\t 3: Promotions directory.\n");
	printf("\t 4: Offers directory.\n");
	printf("\t 5: Marketing directory.\n");
		
	while(1)
	{
		int c = getchar() - '0';
		
		switch(c)
		{
			case 0:
				*length = sizeof("/home/simon/Documents/Assignment2/");
				return "/home/simon/Documents/Assignment2/";
			case 1:
				*length = sizeof("/var/www/html/intra/");
				return "/var/www/html/intra/";
			case 2:
				*length = sizeof("/var/www/html/intra/sales/");
				return "/var/www/html/intra/sales/";
			case 3:
				*length = sizeof("/var/www/html/intra/promotions/");
				return "/var/www/html/intra/promotions/";
			case 4:
				*length = sizeof("/var/www/html/intra/offers/");
				return "/var/www/html/intra/offers/";
			case 5:
				*length = sizeof("/var/www/html/intra/marketing/");
				return "/var/www/html/intra/marketing/";
			default:
				break;
		}
		
		if(c!=-38)
		{
			printf("You entered %d. Please enter a number from 1 to 5",c);
		}
	}
	
	printf("Error: Site directory loop was broken when is shouldn't have been.\n");
	exit(-1);
}

int RecieveDummyMessage(int socketID)
{
	char dummyMessage[SERVER_MESSAGE_LENGTH];
	int errorCode = recv(socketID,dummyMessage,SERVER_MESSAGE_LENGTH,0);
		
	if(errorCode<0)
	{
		printf("Recieve failed. Error code:%d\n",errorCode);
		exit(-1);
	}
}

	

