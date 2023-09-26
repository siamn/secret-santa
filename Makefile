CC = gcc

all: server client

server: pollserver.c participantstruct.h
	$(CC) $^ -o server

client: tcp_client.c userinput.o network.o
	$(CC) $^ -o client

userinput.o: userinput.c userinput.h
	$(CC) -c userinput.c

network.o: network.c network.h
	$(CC) -c network.c

clean:
	rm -f *.o all 