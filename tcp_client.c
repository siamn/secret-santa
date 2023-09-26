#include <time.h>
#include "userinput.h"
#include "network.h"

#define MAX_STR_LIMIT 50
#define QUIT 0

const char *IP = "127.0.0.1";

void sendName(int sd)
{
    printf("What's your name? \n");
    char *name = getLimitedLine(MAX_STR_LIMIT);
    name[strlen(name) + 1] = '\0';
    printf("Sending name: %s\n", name);
    sendStr(name, sd);
}

void sendCommand(int sd, int status)
{
    char *cmd = malloc(MAX_CMD_SIZE);
    int bytes_sent;

    if (status == 1)
    {
        printf("You selected the DRAW option.\n");
        strcpy(cmd, "D ");
        cmd[strlen(cmd + 1)] = '\0';
        // bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
        sendall(sd, cmd, sizeof cmd);
    }
    else if (status == 2)
    {
        printf("You selected the FETCH option.\n");
        strcpy(cmd, "F ");
        cmd[strlen(cmd + 1)] = '\0';
        // bytes_sent = send(sd, cmd, strlen(cmd) + 1, 0);
        sendall(sd, cmd, sizeof cmd);
    }

    free(cmd);
}

void printStatus(int sd, int status)
{
    if (status == 1)
    {
        printf("Drawing was unsuccessful - one participant exists. \n");
    }
    else if (status == 2)
    {
        printf("Draw has already occured! \n");
    }
    else if (status == 3)
    {
        printf("Draw has not happened yet! \n");
    }
    else if (status == 4)
    {
        printf("(Error 405) Invalid data received. Please try again.\n");
    }
    else if (status == 5)
    {
        printf("(Error 406) Invalid data received. Please try again.\n");
    }
    else if (status == 6)
    {
        printf("(Error 407) Insufficient data received.\n");
    }
    else
    {
        printf("Operation was successful!\n");
    }
}

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

    printf("Connected.\n");

    sendName(sd);

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
            sendCommand(sd, 1);

            int statusDraw = receiveData(sd);
            printStatus(sd, statusDraw);
            break;
        case 2:
            sendCommand(sd, 2);

            int statusFetch = receiveLargeData(sd);
            printStatus(sd, statusFetch);
            break;
        default:
            printf("Invalid option!\n");
            break;
        }
    }

    close(sd);
}