#include <netdb.h>
#include <poll.h>
#include <time.h>
#include "network.h"

#define PORT "4242" // Port we're listening on
#define MAX_NUM_OF_CLIENTS 20

// int recvall(int sd, char *buffer, size_t length)
// {
//     while (1)
//     {
//         recv(sd, buffer, length, 0);
//         for (int i = 0; i < strlen(buffer); i++)
//         {
//             if (buffer[i] == '\n')
//             {
//                 if (i + 1 < length)
//                 {
//                     if (buffer[i + 1] == '\0')
//                     {
//                     }
//                 }
//             }
//         }
//     }
// }

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
int get_listener_socket(void)
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
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
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
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

typedef struct Participant
{
    int connIndex; // index of connection for stored participant
    char *name;
    char *giftee;
} P;

typedef struct ParticipantsList
{
    P *arr;
    int length;
} PList;

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

int main(void)
{
    int listener; // Listening socket descriptor

    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[256]; // Buffer for client data

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
    listener = get_listener_socket();

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
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

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

                        del_from_pfds(pfds, i, &fd_count);
                    }
                    else
                    {
                        char firstCharacter = buf[0];
                        switch (firstCharacter)
                        {
                        default: // 0 for input name from client
                            printf("Buffer: %s\n", buf);
                            char *name = malloc(sizeof(char) * 20);
                            strcpy(name, buf + 2);
                            printf("Name: %s\n", name);

                            addParticipant(list, name, i);
                            showParticipants(list);
                            break;
                        case 'D': // 1 for draw request from client
                            if (!drawFlag)
                            {
                                int status = draw(list);
                                if (status == 1)
                                {
                                    char status[2];
                                    strcpy(status, "1");
                                    send(sender_fd, status, strlen(status), 0);
                                    break;
                                }

                                char statusBuf[2];
                                strcpy(statusBuf, "0");
                                send(sender_fd, statusBuf, strlen(statusBuf), 0);
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
                                send(sender_fd, statusBuf, strlen(statusBuf), 0);
                            }
                            break;
                        case 'F': // 2 for fetch request from client
                            if (!drawFlag)
                            {
                                printf("Draw has not happened yet. \n");
                                char status[2];
                                strcpy(status, "3");
                                send(sender_fd, status, strlen(status), 0);
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

    return 0;
}