#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int receiveData(int sv_fd);

int receiveLargeData(int sv_fd);

int parseLargeData(char *buffer, char *save);

void sendStr(char *str, int fd);

/**
 * @brief Function for handling partial sends.
 *
 * @ref https://beej.us/guide/bgnet/html/index-wide.html#sendall
 * @param s
 * @param buf
 * @param len
 * @return int
 */
int sendall(int sd, char *buf, size_t len);