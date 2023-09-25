CC = gcc

all: server client

server: pollserver.c
	$(CC) $^ -o server

client: tcp_client.c userinput.o
	$(CC) $^ -o client

userinput.o: userinput.c userinput.h
	$(CC) -c userinput.c

clean:
	rm -f *.o all 