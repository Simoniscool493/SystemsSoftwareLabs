all : client server
	make cleanobjects

client : client.o fifoWrapper.o
	gcc -o client client.c fifoWrapper.o -lm

server : server.o fifoWrapper.o recentFilesChangedWrapper.o timeWrapper.o
	gcc -o server server.c fifoWrapper.o recentFilesChangedWrapper.o timeWrapper.o -lm

client.o : client.c fifoWrapper.h
	gcc -c client.c
	
server.o : server.c fifoWrapper.h recentFilesChangedWrapper.h timeWrapper.h
	gcc -c server.c

fifoWrapper.o : fifoWrapper.c
	gcc -c fifoWrapper.c
	
recentFilesChangedWrapper.o : recentFilesChangedWrapper.c timeWrapper.h
	gcc -c recentFilesChangedWrapper.c
	
timeWrapper.o : timeWrapper.c
	gcc -c timeWrapper.c

cleanobjects:
	rm client.o fifoWrapper.o server.o recentFilesChangedWrapper.o timeWrapper.o
	
clean:
	rm client client.o fifoWrapper.o server server.o recentFilesChangedWrapper.o timeWrapper.o


