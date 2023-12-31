#include <netdb.h>
#include <poll.h>
#include <time.h>
#include "network.h"
#include "participantstruct.h"

#define PORT "4242" // Port we're listening on
#define MAX_NUM_OF_CLIENTS 20

// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int getListenerSocket(void)
{
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1)
    {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void addToPfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void delFromPfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

void addParticipant(PList *list, char *name, int index)
{
    printf("Participant added.\n");
    list->arr[list->length].name = name;
    list->arr[list->length].connIndex = index;
    list->arr[list->length].giftee = NULL;
    list->length++;
};

void showParticipants(PList *list)
{
    printf("Showing Participants: \n");
    for (int i = 0; i < list->length; i++)
    {
        printf("%d %s %s\n", list->arr[i].connIndex, list->arr[i].name, list->arr[i].giftee);
    }
}

void shuffle(int array[], int length)
{
    srand(time(NULL));
    for (int i = 0; i < length; i++)
    {
        int swap_index = rand() % length;
        int temp = array[i];
        array[i] = array[swap_index];
        array[swap_index] = temp;
    }
}

int draw(PList *list)
{
    if (list->length <= 1)
    {
        return 1;
    }

    // generate array of indexes
    int shuffleArr[list->length];
    for (int i = 0; i < list->length; i++)
    {
        shuffleArr[i] = i;
    }

    shuffle(shuffleArr, list->length);

    for (int i = 0; i < list->length; i++)
    {
        int currIndex = shuffleArr[i];
        P *currP = &list->arr[currIndex];
        int wrappedIndex = (i + 1 >= list->length) ? 0 : i + 1;
        int nextIndex = shuffleArr[wrappedIndex];
        P nextP = list->arr[nextIndex];
        currP->giftee = nextP.name;
        printf("%s: %s\n", list->arr[currIndex].name, list->arr[currIndex].giftee);
    }

    return 0;
}

// looping through as fds are currently deleted from pfds array
// if connections are lost so cannot expect indexes to remain exactly the same
// when indexing directly (rather than searching through loop)
char *fetch(PList *list, int index)
{
    for (int i = 0; i < list->length; i++)
    {
        if (list->arr[i].connIndex == index)
        {
            return list->arr[i].giftee;
        }
    }
    return NULL;
}

void freeMemory(PList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        free(list->arr[i].name);
        free(list->arr[i].giftee);
    }
    free(list->arr);
    free(list);
}

/**
 * @brief Main Server Code
 *
 * @ref Poll example used as starting base https://beej.us/guide/bgnet/html/index-wide.html#poll
 */
int main(void)
{
    int listener; // Listening socket descriptor

    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[MAX_BUF_SIZE]; // Buffer for client data

    PList *list = (PList *)malloc(sizeof(PList)); // Array of structs storing client names and index
    list->arr = (P *)malloc(sizeof(P) * MAX_NUM_OF_CLIENTS);
    list->length = 0;

    char remoteIP[INET6_ADDRSTRLEN];

    int drawFlag = 0;

    // Start off with room for 20 connections
    // (We'll realloc as necessary)
    int fd_count = 0;
    int fd_size = MAX_NUM_OF_CLIENTS;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    // Set up and get a listening socket
    listener = getListenerSocket();

    if (listener == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the listener

    // Main loop

    // Would need to catch a signal for Ctrl-C for freeMemory function to ever be executed.

    while (1)
    {
        // printf("Polling...\n");
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++)
        {
            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == listener)
                {
                    // If listener is ready to read, handle new connection
                    if (drawFlag)
                    {
                        printf("Draw has already happened. Preventing new connection!\n");
                        break;
                    }

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                                   (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        printf("Adding new connection...\n");
                        addToPfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         getInAddr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                    }
                }
                else
                {
                    // If not the listener, we're just a regular client
                    printf("Server waiting...\n");
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;

                    printf("Number of bytes received: %d \n", nbytes);

                    if (nbytes <= 0)
                    {
                        // Got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // Connection closed
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        }
                        else
                        {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        delFromPfds(pfds, i, &fd_count);
                    }
                    else
                    {
                        char firstCharacter = buf[0];
                        switch (firstCharacter)
                        {
                        default: // 0 for input name from client
                            if (drawFlag)
                            {
                                printf("The draw has already been performed. Preventing registration.\n");

                                char status[2];
                                strcpy(status, "7");
                                // send(sender_fd, status, strlen(status), 0);
                                sendall(sender_fd, status, sizeof status);

                                close(pfds[i].fd);
                                delFromPfds(pfds, i, &fd_count);
                                break;
                            }
                            if (list->length < MAX_NUM_OF_CLIENTS)
                            {
                                printf("Buffer: %s\n", buf);
                                char *name = malloc(MAX_STR_SIZE);

                                parseLargeData(buf, name);
                                printf("Received name: %s\n", name);

                                addParticipant(list, name, i);

                                if (list->length >= MAX_NUM_OF_CLIENTS)
                                {
                                    printf("Closing listener socket to prevent incoming connections.\n");
                                    close(listener);
                                }

                                char status[2];
                                strcpy(status, "0");
                                // send(sender_fd, status, strlen(status), 0);
                                sendall(sender_fd, status, sizeof status);

                                showParticipants(list);
                            }
                            else
                            {
                                printf("Max number of participants reached. Preventing registration.\n");

                                char status[2];
                                strcpy(status, "8");
                                // send(sender_fd, status, strlen(status), 0);
                                sendall(sender_fd, status, sizeof status);

                                close(pfds[i].fd);
                                delFromPfds(pfds, i, &fd_count);
                            }
                            break;
                        case 'D': // 1 for draw request from client
                            if (!drawFlag)
                            {
                                int status = draw(list);
                                if (status == 1)
                                {
                                    char status[2];
                                    strcpy(status, "1");
                                    // send(sender_fd, status, strlen(status), 0);
                                    sendall(sender_fd, status, sizeof status);
                                    break;
                                }

                                char statusBuf[2];
                                strcpy(statusBuf, "0");
                                // send(sender_fd, statusBuf, strlen(statusBuf), 0);
                                sendall(sender_fd, statusBuf, sizeof statusBuf);
                                showParticipants(list);

                                close(listener); // Bye!
                                drawFlag = 1;
                            }
                            else
                            {
                                // send back to client message
                                printf("Draw has already happened. \n");
                                char statusBuf[2];
                                strcpy(statusBuf, "2");
                                // send(sender_fd, statusBuf, strlen(statusBuf), 0);
                                sendall(sender_fd, statusBuf, sizeof statusBuf);
                            }
                            break;
                        case 'F': // 2 for fetch request from client
                            if (!drawFlag)
                            {
                                printf("Draw has not happened yet. \n");
                                char status[2];
                                strcpy(status, "3");
                                // send(sender_fd, status, strlen(status), 0);
                                sendall(sender_fd, status, sizeof status);
                                break;
                            }

                            for (int j = 0; j < fd_count; j++)
                            {
                                int dest_fd = pfds[j].fd;
                                if (dest_fd == sender_fd)
                                {
                                    int index = j;
                                    char *name = fetch(list, index);
                                    if (name == NULL)
                                    {
                                        perror("Couldn't fetch giftee name! \n");
                                        break;
                                    }

                                    sendStr(name, dest_fd);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    freeMemory(list);

    return 0;
}