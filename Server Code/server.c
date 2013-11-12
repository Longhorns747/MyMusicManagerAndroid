#include <sys/stat.h>
#include <pthread.h>
#include "data_structs.h"
#include "file_util.h"
#include "networking_util.h"

#define MAX_PENDING 10

void setup_serveraddr(sockaddr_in* serverAddr);
void make_socket(int* sock);
void list(int sock);
void leave(int sock, pthread_t thread);
void pull(int sock);
void diff(int sock);
void *ThreadMain(void *arg);

struct ThreadArgs{
    int clntSock;
};

int main()
{
    int sock;
    make_socket(&sock);

    sockaddr_in serverAddr;
    setup_serveraddr(&serverAddr);

    /* Bind to local address structure */
    if(bind(sock, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0){
        printf("server/main: bind() failed :(\n");
        exit(1);
    }

    /* Listen for incoming connections */
    if(listen(sock, MAX_PENDING)){
        printf("server/main: listen() failed :(\n");
        exit(1);
    }

    while(1)
    {
        /* Accept incoming connection */
        sockaddr_in clientAddr;
        unsigned int clntLen;
        int clientSock;

        clntLen = sizeof(clientAddr);
        clientSock = accept(sock, (struct sockaddr*) &clientAddr, &clntLen);

        if(clientSock < 0){
            printf("server/main:accept() failed :(\n");
            exit(1);
        }

        printf("server/main: Client accepted... \n");

        //Get ready to make a thread!
        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        threadArgs->clntSock = clientSock;

        pthread_t threadID;
        int rtnVal = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
    }
    
    return 0;
}

void *ThreadMain(void* threadArgs)
{
    pthread_detach(pthread_self());

    //Get our socket
    int clientSock = ((struct ThreadArgs *) threadArgs)->clntSock;
    free(threadArgs);

    //Recieve and handle messages
    while(1){
        message msg;
        printf("server/ThreadMain: Ready to recieve messages!\n");
        
        rcv_message(&msg, clientSock);
        printf("server/ThreadMain: Whoa a message! Type: %d\n", msg.type);
        if(msg.type == -1)
            continue;

        switch(msg.type){
            case LEAVE:
                leave(clientSock, pthread_self());
                break;
            case LIST:
                list(clientSock);
                break;
            case DIFF:
                diff(clientSock);
                break;
	        case PULL:
		        pull(clientSock);
                break;
        }

    }
}

void make_socket(int* sock)
{
    /* Create new TCP Socket for incoming requests*/
    in_port_t serverPort = htons(PORT);
    if((*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        printf("server/make_socket: socket() failed :(\n");
    }

    //Set socket options so that we can release the port
    int on = 1;
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void setup_serveraddr(sockaddr_in* serverAddr)
{
    /* Construct local address structure*/
    memset(serverAddr, 0, sizeof(*serverAddr));

    //Protocol family, address, port
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr->sin_port = htons(PORT);
}

void list(int sock)
{
    char msg[] = {"server/list: Doing a LIST :O\n"};
    FILE* file = fopen(LOGNAME, "ab");
    fputs(msg, file);
    printf("%s", msg);
    fclose(file);

    //Get current filestate
    filestate currState;
    update_files(&currState);
    send_filenames(&currState, sock);
}

void leave(int sock, pthread_t thread)
{
    char msg[] = {"server/leave: Doing a LEAVE :O\n"};
    FILE* file = fopen(LOGNAME, "ab");
    fputs(msg, file);
    printf("%s", msg);
    fclose(file);

    close(sock);
    pthread_cancel(thread);
}

void pull(int sock)
{
    char msg[] = {"server/pull: Doing a PULL :O\n"};
    FILE* file = fopen(LOGNAME, "ab");
    fputs(msg, file);
    fclose(file);

    printf("%s", msg);
    filestate currState;
    filestate senderIDs;
    filestate diff;

    update_files(&currState);
    rcv_IDs(&senderIDs, sock);
    delta(&senderIDs, &currState, &diff);
    send_music_files(&diff, sock);
}

void diff(int sock)
{
    char msg[] = {"server/diff: Doing a DIFF :O\n"};
    FILE* file = fopen(LOGNAME, "ab");
    fputs(msg, file);
    fclose(file);

    printf("%s", msg);
    filestate serverState;
    filestate clientState;
    filestate diff;

    update_files(&serverState);

    rcv_IDs(&clientState, sock);
    delta(&clientState, &serverState, &diff);
    send_filenames(&diff, sock);
}

