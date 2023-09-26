#include "network.h"

#define MAX_SIZE 100

int checkForErrorCodes(char *buffer)
{
    // Error Codes
    // For draw:
    // 1 = Insufficient clients registered
    // 2 = Draw has already happened

    // For fetch:
    // 3 = Draw has not happened yet
    //
    if (strcasecmp(buffer, "1") == 0)
    { // 1 corresponding to an error status received
        return 1;
    }

    else if (strcasecmp(buffer, "2") == 0)
    { // 2 corresponding to a draw already taking place
        return 2;
    }

    else if (strcasecmp(buffer, "3") == 0)
    {
        return 3;
    }

    return 0;
}

int receiveData(int sv_fd)
{
    char buffer[MAX_SIZE];
    bzero(buffer, MAX_SIZE);
    printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);
    printf("From server: %s\n", buffer);

    return checkForErrorCodes(buffer);
}

int receiveLargeData(int sv_fd)
{
    char buffer[MAX_SIZE];
    bzero(buffer, MAX_SIZE);
    // printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);

    int res = checkForErrorCodes(buffer);
    if (res)
    {
        printf("Errored early! %d\n", res);
        return res; // there was an error, return the correct code
    }

    char *s1;
    char *s2;
    char *sp;
    char *end;
    // printf("Received: %s\n", buffer);
    sp = strchr(buffer, ' ');
    if (!sp)
    {
        return 4;
    }

    s1 = strndup(buffer, sp - buffer); // copy until space
    s2 = sp + 1;                       // skip space for next part of string
    char *data = strndup(s2, strlen(buffer));

    // printf("Buffer: %s\n", buffer);
    int length = strtol(s1, &end, 10);
    // printf("Size: %s", s1);
    if (end == s1)
    {
        return 5;
    }
    else if (length != strlen(data) + 2) // need to account for space and null terminator
    {
        // printf("Length (%d) + len data: %lu", length, strlen(data) + 2);
        return 6;
    }
    printf("Received: %s\n", data);
    // printf("Length of data: %lu\n", strlen(data) + 2);

    return 0;
}
