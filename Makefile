CC = gcc

all: server client

server: pollserver.c
	$(CC) $^ -o server

client: tcp_client.c
	$(CC) $^ -o client

clean:
	rm -f *.o all 