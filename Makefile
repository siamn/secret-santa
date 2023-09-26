CC = gcc

all: server client

server: pollserver.c network.o participantstruct.h
	$(CC) pollserver.c network.o -o server 

client: tcp_client.c userinput.o network.o
	$(CC) $^ -o client

userinput.o: userinput.c userinput.h
	$(CC) -c userinput.c

network.o: network.c network.h
	$(CC) -c network.c

clean:
	rm -f *.o all 