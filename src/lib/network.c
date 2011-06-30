/*
 * Copyright (c) 2010 Ignasi Barrera
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include "network.h"


void net_connect(char* address, int port) {
    struct hostent *host_entry;     // Host name
    struct sockaddr_in sock_addr;   // Remote address

    // Get remote host address
    if ((host_entry = gethostbyname(address)) == NULL) {
        perror("gethostbyname error");
        exit(EXIT_FAILURE);
    }

    // Write zeros into remote address structure
    memset(&sock_addr, 0, sizeof(sock_addr));

    // Family type, server port and host address
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr = *((struct in_addr *) host_entry->h_addr);

    // Socket creation
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // Create a connection with the remote host
    if (connect(_socket, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }
}

void net_disconnect() {
    close(_socket);
    _socket = -1;
}

int net_send(char* msg) {
    char out[MSG_SIZE];

    strncpy(out, msg, MSG_SIZE - 2);    // Cut the message to the maximum size
    strcat(out, MSG_SEP);               // Messages must end like this

    printf(">> %s\n", msg);

    return send(_socket, out, strlen(out), 0);
}

int net_recv(char* msg) {
    int numbytes;

    if ((numbytes = recv(_socket, msg, MSG_SIZE - 1, 0)) > 0) {
        msg[numbytes] = '\0';   // Append the "end of string" character
    }

    printf("<< %s\n", msg);

    return numbytes;
}

int net_listen() {
    int read, ret;
    fd_set read_fd_set;

    // Initialize the set of active sockets
    FD_ZERO(&read_fd_set);
    FD_SET(_socket, &read_fd_set);

    // Check if there is some data to be read (avoid blocking read)
    read = select(_socket + 1, &read_fd_set, NULL, NULL, NULL);

    // If there is an error in select, abort except
    // if the error is an interrupt signal. We'll just
    // ignore it since we are handling the signals.
    if (read < 0) {
        ret = (errno == EINTR)? NET_CLOSE : NET_ERROR;
    } else if (read > 0 && FD_ISSET(_socket, &read_fd_set)) {
        ret = NET_READY;
    } else {
        ret = NET_IGNORE;
    }

    return ret;
}

