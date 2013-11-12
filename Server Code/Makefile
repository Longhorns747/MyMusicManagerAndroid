	CC=gcc

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: server 

server: server.c
	$(CC) server.c -ggdb -o musicServer -std=c99 -lcrypto -lpthread

clean:
	    rm -f server *.o
