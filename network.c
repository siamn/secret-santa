#include "network.h"

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
    char buffer[MAX_CMD_SIZE];
    bzero(buffer, MAX_CMD_SIZE);
    printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);
    printf("From server: %s\n", buffer);

    return checkForErrorCodes(buffer);
}

int parseLargeData(char *buffer, char *save)
{
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
    if (save != NULL)
    {
        printf("Saving....\n");
        strcpy(save, data);
    }

    free(s1);
    free(data);

    return 0;
}

int receiveLargeData(int sv_fd)
{
    char buffer[MAX_STR_SIZE];
    bzero(buffer, MAX_STR_SIZE);
    // printf("Waiting...\n");
    recv(sv_fd, buffer, sizeof(buffer), 0);

    int res = checkForErrorCodes(buffer);
    if (res)
    {
        printf("Errored early! %d\n", res);
        return res; // there was an error, return the correct code
    }

    return parseLargeData(buffer, NULL);
}

void sendStr(char *str, int fd)
{
    char buf[strlen(str) + 3];
    // Siam - 4
    sprintf(buf, "%lu ", strlen(str) + 2);
    strcat(buf, str);
    printf("Sending (buf): %s \n", buf);

    printf("Sending (str) %s \n", str);
    if (send(fd, buf, sizeof buf, 0) == -1)
    {
        perror("send");
    }
}

int sendall(int sd, char *buf, size_t len)
{
    int total = 0;       // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;

    while (total < len)
    {
        n = send(sd, buf + total, bytesleft, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}