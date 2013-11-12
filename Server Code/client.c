#include <string.h>
#include "data_structs.h"
#include "file_util.h"
#include "networking_util.h"

int setup_connection(sockaddr_in* address);
void setup_addr(char* IPaddr, sockaddr_in *address);
int user_prompt();
void leave(int sock);
void pull(int sock);
void list(int sock);
void diff(int sock);

int main()
{
	int userChoice = 1;

	//Set up a connection to the server
	sockaddr_in serverAddr;
	setup_addr("130.207.114.21", &serverAddr);
	int sock = setup_connection(&serverAddr);

	printf("Connection to server established\n");
	printf("Welcome to your Music Manager :D!\n");

	int lastPacket = 1;

	while(userChoice)
	{
		userChoice = user_prompt();

		//Handle bad user input
		if(userChoice == -1)
			continue;

		message msg;
		
		//Fill message struct
		create_message(&msg, 0, userChoice, LAST_PACKET, 0);
		send_message(&msg, sock);

		switch(msg.type){
		    case LEAVE:
		        leave(sock);
		        break;
		    case LIST:
		        list(sock);
		        break;
		    case DIFF:
		    	diff(sock);
		    	break;
		    case PULL:
		        pull(sock);
                break;
		}
	}

	return 0;
}

int user_prompt()
{
	char selection[5];
	printf("Please make a selection from one of the following: LEAVE, LIST, PULL, DIFF\n");
	scanf("%s", selection);

	int select = -1;

	for(int i = 0; i < NUM_MESSAGES; i++){
		if(!strcmp(selection, selections[i])){
			select = i;
		}
	}

	//Fix it with ductape!!!
	if(!strcmp(selection, "LEAVE"))
		select = 0;

	return select;
}

//Set up the address structure for a given address
void setup_addr(char* IPaddr, sockaddr_in *address)
{
	memset(address, 0, sizeof(*address));

	//Setting the protocol family
    address->sin_family = AF_INET;

    //Formatting and setting the IP address
    int rtnVal = inet_pton(AF_INET, IPaddr, &(address->sin_addr.s_addr));
    if(rtnVal <= 0){
        printf("client/setup_addr: inet_pton failed :(\n");
        exit(1);
    }

    //Setting the port
    address->sin_port = htons(PORT);
}

//Returns a socket to setup the connection with a host
int setup_connection(sockaddr_in* address)
{
    /* Create a new TCP socket*/
    int clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(clientSock < 0){
        printf("socket() failed :(\n");
        exit(1);
    }

    if(connect(clientSock, (struct sockaddr *) address, sizeof(*address)) < 0){
        printf("Server is down, slap Ethan! :(\n");
        exit(1);
    }

    return clientSock;
}

void leave(int sock)
{
	close(sock);
	exit(1);
}

void pull(int sock)
{
    filestate currState;
    update_files(&currState);
    send_ids(&currState, sock);
    
    rcv_music_files(sock);	
    printf("All Done!\n");
}

void list(int sock)
{
	rcv_filenames(sock);
	printf("All Done!\n");
}

void diff(int sock)
{
	filestate currState;
	update_files(&currState);
	send_ids(&currState, sock);
	rcv_filenames(sock);
	printf("All Done!\n");
}
