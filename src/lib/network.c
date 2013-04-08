/*
 * Copyright (c) 2011 Ignasi Barrera
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define _POSIX_SOURCE   /* Required for fdopen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include "debug.h"
#include "network.h"


int _socket;                /* The socket to the IRC server */
static FILE* _sd = NULL;    /* A file pointer to be able to read from the socket line by line */

void net_connect(char* address, int port) {
    struct hostent *host_entry;     /* Host name */
    struct sockaddr_in sock_addr;   /* Remote address */

    /* Get remote host address */
    debug(("network: Resolving address: %s\n", address));
    if ((host_entry = gethostbyname(address)) == NULL) {
        perror("gethostbyname error");
        exit(EXIT_FAILURE);
    }

    /* Write zeros into remote address structure */
    memset(&sock_addr, 0, sizeof(sock_addr));

    /* Family type, server port and host address */
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr = *((struct in_addr *) host_entry->h_addr_list[0]);

    /* Socket creation */
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    /* Create a connection with the remote host */
    debug(("network: Connecting\n"));
    if (connect(_socket, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    _sd = fdopen(_socket, "r");     /* Create the file descriptor to read from the socket line by line */
    setvbuf(_sd, NULL, _IONBF, 0);  /* Turn off buffering to avoid blocking input */
}

void net_disconnect() {
    debug(("network: Disconnecting\n"));
    close(_socket);
    _socket = 0;
    _sd = NULL;
}

int net_send(char* msg) {
    char out[READ_BUF];             /* The real size we can send in the socket, considering the '\0'*/

    strncpy(out, msg, WRITE_BUF);   /* Cut the message to the maximum size */
    out[WRITE_BUF]= '\0';           /* Make sure string is null terminated. Perhaps the '\0' was stripped) */
    strcat(out, MSG_SEP);           /* Messages must end like this */

    printf(">> %s", out);

    return send(_socket, out, strlen(out), 0);
}

void net_recv(char* msg) {
    char* ret;

    /* Read only a single line from the socket */
    ret = fgets(msg, READ_BUF, _sd);
    msg[READ_BUF - 1] = '\0';   /* Make sure the string is properly terminated */

    if (ret == NULL) {
        perror("Error reading from socket");
        exit(EXIT_FAILURE);
    }

    printf("<< %s", msg);
}

enum net_status net_listen() {
    int read, ret;
    fd_set read_fd_set;

    /* Initialize the set of active sockets */
    FD_ZERO(&read_fd_set);
    FD_SET(_socket, &read_fd_set);

    /* Check if there is some data to be read */
    read = select(_socket + 1, &read_fd_set, NULL, NULL, NULL);

    /* If there is an error in select, abort except
     * if the error is an interrupt signal. We'll just
     * ignore it since we are handling the signals. */
    if (read < 0) {
        debug(("network: Select returned %d\n", errno));
        ret = (errno == EINTR)? NET_CLOSE : NET_ERROR;
    } else if (read > 0 && FD_ISSET(_socket, &read_fd_set)) {
        ret = NET_READY;
    } else {
        /* This should never be raised, because no timeout is defined */
        ret = NET_TIMEOUT;
    }

    return ret;
}

