#include "network.h"

#define MAX_SIZE 100

void receiveData(int sv_fd)
{
    char buffer[MAX_SIZE];
    bzero(buffer, MAX_SIZE);
    printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);
    printf("From server: %s\n", buffer);
}

void receiveLargeData(int sv_fd)
{
    char buffer[MAX_SIZE];
    bzero(buffer, MAX_SIZE);
    // printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);
    char *s1;
    char *s2;
    char *sp;
    char *end;
    // printf("Received: %s\n", buffer);
    sp = strchr(buffer, ' ');
    if (!sp)
    {
        // printf("Received: %s\n", buffer);
        printf("(Error 405) Invalid data received. Please try again.\n");
        return;
    }

    s1 = strndup(buffer, sp - buffer); // copy until space
    s2 = sp + 1;                       // skip space for next part of string
    char *data = strndup(s2, strlen(buffer));

    // printf("Buffer: %s\n", buffer);
    int length = strtol(s1, &end, 10);
    // printf("Size: %s", s1);
    if (end == s1)
    {
        printf("(Error 406) Invalid data received. Please try again.\n");
        return;
    }
    else if (length != strlen(data) + 2) // need to account for space and null terminator
    {
        // printf("Length (%d) + len data: %lu", length, strlen(data) + 2);
        printf("(Error 407) Insufficient data received.\n");
        return;
    }
    printf("Received: %s\n", data);
    // printf("Length of data: %lu\n", strlen(data) + 2);
}
