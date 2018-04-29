#ifndef REMOTESERVERWRAPPER_H
#define REMOTESERVERWRAPPER_H

struct UserAccount
{ 
    char* username;
	char* password;
};

int WriteFile(char* fileName,char* filePath,char* fileData);
int SendDummyMessage(int socketID);
int DoesFileHaveEnd(char* fileData);
int HandleSingleClient(int clientSocket);
int RunRemoteServer();
struct UserAccount* GetUsernamesAndPasswords(int* length);


#endif
