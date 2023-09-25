#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "userinput.h"

#define MAX_SIZE 100
const char *IP = "127.0.0.1";

void showTime(int sv_fd)
{
    char buffer[MAX_SIZE];
    while (1)
    {
        bzero(buffer, MAX_SIZE);
        printf("Connected.\n");
        printf("Waiting...\n");
        recv(sv_fd, buffer, sizeof(buffer), 0);
        printf("From server: %s\n", buffer);
        printf("Waiting...\n");
    }
}

void main_menu()
{
    printf("\n ------------------------------------ \n");
    printf("Welcome to the Secret Santa Project! \n");
    printf("Please enter an option from the list shown below: \n");
    printf("'draw' for to begin pairing up with a giftee \n");
    printf("'fetch' for printing out who your giftee is \n");
    printf("'help' for more information \n");
    printf("'0' to EXIT. \n");
    printf("------------------------------------ \n");
}

int main()
{
    int ret, sd, bytes_sent;
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

        char *name = malloc(50);
        strcpy(name, "0 ");
        printf("What's your name? \n");
        // while loop for name validation (no numbers etc...)
        char *input = getLimitedLine(47);
        // num _ name /0
        strlcat(name, input, 50);
        name[strlen(name) + 1] = '\0';
        printf("Sending name: %s\n", name);
        bytes_sent = send(sd, name, strlen(name) + 1, 0);
        printf("Bytes sent: %d\n", bytes_sent);

        char *cmd = malloc(10);
        char *inputCmd = getLimitedLine(7);
        printf("You typed %s", inputCmd);
        if (strcasecmp(inputCmd, "draw") == 0)
        {
            printf("You triggered a draw!!\n");
            strcpy(cmd, "1 ");
            cmd[strlen(cmd + 1)] = '\0';
            bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
            printf("Bytes sent: %d\n", bytes_sent);
        }
        else if (strcasecmp(inputCmd, "fetch") == 0)
        {
            printf("You triggered a fetch!!\n");
            strcpy(cmd, "2 ");
            cmd[strlen(cmd + 1)] = '\0';
            bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
            printf("Bytes sent: %d\n", bytes_sent);
        }

        // while (1)
        // {

        //     main_menu();
        //     // scanf("%s", input);
        //     input = getLimitedLine(20);

        //     if (strcasecmp(input, "draw") == 0)
        //     {
        //         time_t currentTime = time(NULL);
        //         char *time_msg = asctime(localtime(&currentTime));
        //         printf("Sending time to client: %s \n", time_msg);
        //         bytes_sent = send(sd, time_msg, strlen(time_msg), 0);
        //         printf("Bytes sent: %d\n", bytes_sent);
        //     }
        // }
        free(input);
    }

    showTime(sd);

    close(sd);
}