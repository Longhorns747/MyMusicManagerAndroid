#ifndef DATA_STRUCTS_H_INCLUDED
#define DATA_STRUCTS_H_INCLUDED

#include <stdio.h>		    /* for printf() and fprintf() */
#include <stdlib.h>
#include <sys/socket.h>     /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>      /* for sockaddr_in and inet_addr() */
#include <stdbool.h>

int NUM_MESSAGES = 4;
char selections[4][5] = {"LEAVE", "LIST", "PULL", "DIFF"};

typedef enum {
	LEAVE, LIST, PULL, DIFF
} message_type;

typedef struct 
{
	char* filename;
	unsigned char* ID;
} music_file;

typedef struct
{
	struct sockaddr_in hostIP;
	int numFiles;
	music_file* music_files;
} filestate;

typedef struct
{
	int type; //Enum for message type
	int num_bytes; //The number of bytes to be sent
	int last_message; //Is this the last message from the sender?
	int filename_length;
} message;

#endif
