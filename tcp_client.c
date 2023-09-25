#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_SIZE 100
const char *IP = "127.0.0.1";

void showTime(int sv_fd)
{
    char buffer[MAX_SIZE];
    while (1)
    {
        bzero(buffer, MAX_SIZE);
        // printf("Connected.\n");
        recv(sv_fd, buffer, sizeof(buffer), 0);
        printf("From server: %s\n", buffer);
        printf("Waiting...\n");
    }
}

int main()
{
    int ret, sd;
    struct sockaddr_in sv_addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(4242);
    inet_pton(AF_INET, IP, &sv_addr.sin_addr);

    ret = connect(sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));

    if (ret != 0)
    {
        perror("Could not connect to the server.\n");
        exit(0);
    }
    else
    {
        printf("Connected.\n");
    }

    showTime(sd);

    close(sd);
}