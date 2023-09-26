#include <time.h>
#include "userinput.h"
#include "network.h"

#define MAX_SIZE 100
#define QUIT 0

const char *IP = "127.0.0.1";

void main_menu()
{
    printf("\n ------------------------------------ \n");
    printf("Welcome to the Secret Santa Project! \n");
    printf("Please enter an option from the list shown below: \n");
    printf("1. 'draw' for to begin pairing up with a giftee \n");
    printf("2. 'fetch' for printing out who your giftee is \n");
    printf("3. 'help' for more information \n");
    printf("0. to EXIT. \n");
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
        // char *inputCmd = getLimitedLine(7);
        // printf("You typed %s", inputCmd);
        // if (strcasecmp(inputCmd, "draw") == 0)
        // {
        //     printf("You triggered a draw!!\n");
        //     strcpy(cmd, "1 ");
        //     cmd[strlen(cmd + 1)] = '\0';
        //     bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
        //     printf("Bytes sent: %d\n", bytes_sent);
        // }
        // else if (strcasecmp(inputCmd, "fetch") == 0)
        // {
        //     printf("You triggered a fetch!!\n");
        //     strcpy(cmd, "2 ");
        //     cmd[strlen(cmd + 1)] = '\0';
        //     bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
        //     printf("Bytes sent: %d\n", bytes_sent);
        // }
        int option = -1;

        while (option != QUIT)
        {
            main_menu();
            option = getPositiveInt();

            if (option < 0)
            {
                printf("Your provided input is not valid. Please try again.");
                continue;
            }

            switch (option)
            {
            case 0:
                printf("Quitting...\n");
                break;
            case 1:
                printf("You triggered a draw!!\n");
                strcpy(cmd, "1 ");
                cmd[strlen(cmd + 1)] = '\0';
                bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
                printf("Bytes sent: %d\n", bytes_sent);

                break;
            case 2:
                printf("You triggered a fetch!\n");
                strcpy(cmd, "2 ");
                cmd[strlen(cmd + 1)] = '\0';
                bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
                printf("Bytes sent: %d\n", bytes_sent);
                receiveLargeData(sd);
                break;

            default:
                printf("Invalid option!\n");
                break;
            }
        }
        free(input);
        // showTime(sd);

        close(sd);
    }
}