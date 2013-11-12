#ifndef NETWORKING_UTIL_H
#define NETWORKING_UTIL_H

#include <sys/socket.h>     /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>      /* for sockaddr_in and inet_addr() */
#include <string.h>
#include "data_structs.h"

#define LAST_PACKET 1
#define NOT_LAST_PACKET 0
#define PORT 2222
#define BUFSIZE 1500
#define MESSAGESIZE sizeof(message)

typedef struct sockaddr_in sockaddr_in;

void create_message(message* msg, int numBytes, int msgType, int last_message, int filename_length);
void send_message(message* msg, int sock);
void send_payload(int size, byte* payload, int sock);
void rcv_message(message* msg, int sock);
void send_filenames(filestate* state, int sock);
void rcv_filenames(int sock);
void send_ids(filestate* state, int sock);
void rcv_IDs(filestate* res, int sock);
void send_music_files(filestate* state, int sock);
void rcv_music_files(int sock);

void create_message(message* msg, int numBytes, int msgType, int last_message, int filename_length)
{
    msg->num_bytes = numBytes;
    msg->type = msgType;
    msg->last_message = last_message;
    msg->filename_length = filename_length;
}

void send_message(message* msg, int sock)
{
    ssize_t len = sizeof(message);

    send(sock, msg, len, 0);
}

void rcv_message(message* msg, int sock)
{
    byte rcvBuf[sizeof(message)];

    //Recieve metadata from client
    ssize_t bytesRecieved;
    int totalBytes = 0;

    while(totalBytes < sizeof(message)){
        bytesRecieved = recv(sock, rcvBuf, sizeof(message), 0);

        if(bytesRecieved < 0)
            perror("Recv failed :(");

        totalBytes += bytesRecieved;
    }

    memcpy(msg, rcvBuf, sizeof(message));
}

//Send the entire payload in 1 or more packets
void send_payload(int size, byte* payload, int sock)
{
    int remainingBytes = size;
    int offset = 0;
    int numSent;

    //Keep sending packets while the number of bytes left is > BUFSIZE
    while(remainingBytes > BUFSIZE){
        numSent = send(sock, &payload[BUFSIZE*offset], BUFSIZE, 0);

        if(numSent < 0)
            perror("send() failed");

        remainingBytes = remainingBytes - numSent;
        offset=offset+1;                
    }

    //Send the remainder of the payload
    if(send(sock, &payload[BUFSIZE*offset], remainingBytes, 0) != remainingBytes)
       perror("send() sent unexpected number of bytes of data");       
}

//
void send_filenames(filestate* state, int sock)
{
    //send a metadata packet for each filename, then send the filename
    for(int i = 0; i < state->numFiles; i++){
        char* filename = state->music_files[i].filename;

    	//create and send metadata message
    	message msg;
        create_message(&msg, strlen(filename), -1, NOT_LAST_PACKET, strlen(filename)); //+1 for '/0'?
    	send_message(&msg, sock);

	    //send filename
        send_payload(msg.num_bytes, filename, sock);
    }

    //Make the last message to 
    message lastMsg;
    create_message(&lastMsg, 0, DIFF, LAST_PACKET, 0);
    send_message(&lastMsg, sock);
}

void send_music_files(filestate* state, int sock)
{
    //send a metadata packet for each file, then send the file
    for(int i = 0; i < state->numFiles; i++){
        char* filename = state->music_files[i].filename;

    	//load each file
	    struct stat fileAttributes;

    	stat(filename, &fileAttributes);
    	byte* file = load_file(filename, fileAttributes.st_size); 

    	//create and send metadata message
    	message msg;
        create_message(&msg,fileAttributes.st_size, -1, NOT_LAST_PACKET, strlen(filename)); //+1?
	    send_message(&msg, sock);

        //send the filename payload
	    send_payload(strlen(filename), filename, sock);

    	//send music file
    	printf("ntwkutil/send_music_files: Start sending payload\n");
        send_payload(fileAttributes.st_size, file, sock);
	    printf("ntwkutil/send_music_files: Finished sending payload\n");
    }
    
    //Make the last message
    message lastMsg;
    create_message(&lastMsg, 0, PULL, LAST_PACKET, 0);
    send_message(&lastMsg, sock);
}


void rcv_music_files(int sock)
{
    printf("Waiting for music files...\n");
    message msg;
    rcv_message(&msg, sock);	 
   
    //Keep receiveing until we receive a last packet flag 
    while(!msg.last_message){
    	//receive filename packet
    	char filename[msg.filename_length];
    	recv(sock, &filename, msg.filename_length, 0);	

        filename[msg.filename_length] = '\0';	
        printf("Receiving: %s...\n", filename, msg.num_bytes);
        byte* rcvMsg = (byte *) malloc(sizeof(byte)*msg.num_bytes);

        int numBytesExpected = msg.num_bytes;
        int numBytesRecv = 0;
        int totalBytes = 0;
        int remainingBytes = numBytesExpected;

    	//Keep receiving full packets until packet is not full
    	while(totalBytes < (numBytesExpected)){
    	   numBytesRecv = recv(sock, &rcvMsg[totalBytes], remainingBytes, 0);
           totalBytes += numBytesRecv;
           remainingBytes -= numBytesRecv;
	    }

        //Now we have the whole file. Save it 
        save_file(rcvMsg, msg.num_bytes, filename);
    	//Next we expect a message packet
    	rcv_message(&msg, sock);
    }
}

void rcv_filenames(int sock)
{
    printf("Waiting for a list of files...\n");
    message msg;
    rcv_message(&msg, sock);
    int count = 1;

    while(!msg.last_message)
    {
        //Recieve filename from server
        byte filename[msg.num_bytes + 1];
        memset(filename, 0, msg.num_bytes);

        ssize_t bytesRecieved;
        int totalBytes = 0;

        while(totalBytes < msg.num_bytes){
            bytesRecieved = recv(sock, filename, msg.num_bytes, 0);
            if(bytesRecieved < 0)
                printf("Couldn't recieve filenames :(");

            totalBytes += bytesRecieved;
        }

        filename[msg.num_bytes] = '\0';
        printf("File %d: %s\n", count, (char *)filename);
        count++;

        memset(filename, 0, msg.num_bytes);
        rcv_message(&msg, sock);
    }

    if(count == 1)
        printf("All files are the same!\n");
}

//NOTE TO SELF: what if filestate takes up more than one packet?
void send_ids(filestate* state, int sock)
{	
    
    for(int i = 0; i < state->numFiles; i++){
        char* ID = state->music_files[i].ID;

        message msg;
        create_message(&msg, strlen(ID), -1, NOT_LAST_PACKET, 0);
	    send_message(&msg, sock);

        send_payload(strlen(ID), ID, sock);
    }

    //Make the last message
    message lastMsg;
    create_message(&lastMsg, 0, -1, LAST_PACKET, 0);
    send_message(&lastMsg, sock);
}

void rcv_IDs(filestate* res, int sock)
{
    printf("ntwkutil/rcv_ids:  Waiting for file IDs...\n");
    message msg;
    rcv_message(&msg, sock);
    int count = 0;

    music_file* fileList;
    fileList = (music_file*) malloc(sizeof(music_file));

    while(!msg.last_message)
    {
        fileList = (music_file*) realloc(fileList, sizeof(music_file)*(count + 1)); 

        //Recieve ID from server
        byte ID[msg.num_bytes];
        
        //memset(ID, 0, msg.num_bytes + 1);
        ssize_t bytesRecieved;
        int totalBytes = 0;

        while(totalBytes < msg.num_bytes){
            bytesRecieved = recv(sock, ID, msg.num_bytes, 0);

            if(bytesRecieved < 0)
                printf("ntwkutil/rcv_ids: Couldn't recieve ID :(\n");

            totalBytes += bytesRecieved;
        }

        music_file* currFile = (music_file*) malloc(sizeof(music_file));
        currFile->ID = (unsigned char *) malloc(sizeof(ID));
        memcpy(currFile->ID, ID, sizeof(ID));

        fileList[count] = *currFile;

        count++;
        rcv_message(&msg, sock);
    }

    res->music_files = fileList;
    res->numFiles = count;
}

#endif
