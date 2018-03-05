#ifndef FIFOWRAPPER_H
#define FIFOWRAPPER_H

int ReadFIFO(int fileHandle);
int OpenFIFO(const char* path);
int CreateFIFO(const char* path);

#endif

