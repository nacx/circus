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

#define _POSIX_C_SOURCE 200112L      /* Use addr structs and fdopen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <err.h>
#include <netdb.h>
#include <unistd.h>
#include "debug.h"
#include "network.h"


int _socket = -1;           /* The socket to the IRC server */
static FILE* _sd = NULL;    /* A file pointer to be able to read from the socket line by line */

void net_connect(char* address, char* port) {
    struct addrinfo addr_in;            /* Remote address configuration */
    struct addrinfo *addr_out, *addr;   /* Resolved addresses */
    int error;                          /* Error number when resolving host name */

    /* Get remote host address */
    debug(("network: Resolving address: %s\n", address));
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.ai_family = AF_INET;
    addr_in.ai_socktype = SOCK_STREAM;

    error = getaddrinfo(address, port, &addr_in, &addr_out);
    if (error) {
        errx(EXIT_FAILURE, "%s", gai_strerror(error));
    }

    debug(("network: Connecting\n"));
    for (addr = addr_out; addr && _socket == -1; addr = addr->ai_next) {
        if ((_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) != -1) {
            if (connect(_socket, addr->ai_addr, addr->ai_addrlen) != -1) {
                /* Create the file descriptor to read from the socket line by line
                 * and turn off buffering to avoid blocking input */
                _sd = fdopen(_socket, "r");
                setvbuf(_sd, NULL, _IONBF, 0);
            } else {
                /* If connection fails, close the socket and try next resolved address */
                close(_socket);
                _socket = -1;
            }
        }
    }

    freeaddrinfo(addr_out);     /* Free the temporal address info */

    if (_socket == -1) {
        errx(EXIT_FAILURE, "Could not connect to: %s:%s\n", address, port);
    }
}

void net_disconnect() {
    debug(("network: Disconnecting\n"));
    close(_socket);
    _socket = -1;
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

    debug(("network: Waiting for incoming messages...\n"));

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
        debug(("network: Timeout. Socket dead?\n"));
        ret = NET_TIMEOUT;
    }

    return ret;
}

