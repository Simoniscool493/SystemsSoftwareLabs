all : client server remoteClient
	make cleanobjects

client : client.o fifoWrapper.o
	gcc -o client client.c fifoWrapper.o -lm

server : server.o fifoWrapper.o recentFilesChangedWrapper.o timeWrapper.o remoteServerWrapper.o 
	gcc -o server server.c fifoWrapper.o recentFilesChangedWrapper.o timeWrapper.o remoteServerWrapper.o -lm

remoteClient : remoteClient.o
	gcc -o remoteClient remoteClient.c -lm

client.o : client.c fifoWrapper.h
	gcc -c client.c
	
server.o : server.c fifoWrapper.h recentFilesChangedWrapper.h timeWrapper.h remoteServerWrapper.h
	gcc -c server.c

remoteClient.o : remoteClient.c
	gcc -c remoteClient.c
	
remoteServerWrapper.o : remoteServerWrapper.c recentFilesChangedWrapper.h timeWrapper.h
	gcc -c remoteServerWrapper.c

recentFilesChangedWrapper.o : recentFilesChangedWrapper.c timeWrapper.h
	gcc -c recentFilesChangedWrapper.c

timeWrapper.o : timeWrapper.c
	gcc -c timeWrapper.c
	
fifoWrapper.o : fifoWrapper.c
	gcc -c fifoWrapper.c

cleanobjects:
	rm client.o fifoWrapper.o server.o remoteClient.o remoteServerWrapper.o recentFilesChangedWrapper.o timeWrapper.o
	
clean:
	rm client.o fifoWrapper.o server.o remoteClient.o remoteServerWrapper.o recentFilesChangedWrapper.o timeWrapper.o client server remoteClient


