	CC=gcc

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: client server 

client: client.c
	$(CC) client.c -g -o musicManager -std=c99 -lcrypto

server: server.c
	$(CC) server.c -ggdb -o musicServer -std=c99 -lcrypto -lpthread

clean:
	    rm -f client server *.o
