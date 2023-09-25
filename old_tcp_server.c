#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <time.h>

#define MAX_SIZE 100
const char *IP = "127.0.0.1";

void sendTime(int cl_fd)
{
    char buffer[MAX_SIZE];
    while (1)
    {
        sleep(5);
        bzero(buffer, MAX_SIZE);
        time_t sys_time = time(0);
        char *time = ctime(&sys_time);
        time[strlen(time) - 1] = '\0';
        snprintf(buffer, MAX_SIZE, "The current time is %s", time);
        send(cl_fd, buffer, sizeof(buffer), 0);
    }
}

int main()
{
    int ret, sd, new_sd;
    socklen_t len;
    struct sockaddr_in my_addr, cl_addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4242);
    inet_pton(AF_INET, IP, &my_addr.sin_addr);

    ret = bind(sd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    ret = listen(sd, 10);
    len = sizeof(cl_addr);
    new_sd = accept(sd, (struct sockaddr *)&cl_addr, &len);
    printf("Got here\n");
    sendTime(new_sd);

    close(sd);
    // close(new_sd);
}