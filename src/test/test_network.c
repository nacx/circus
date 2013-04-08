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

#define _POSIX_SOURCE /* Required for fdopen in network.c */

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <poll.h>
#include "minunit.h"
#include "test.h"
#include "../lib/utils.h"
#include "../lib/network.c"

#define TEST_PORT 19876

/* Start a mock server that returns after accepting a connection */
void* mock_server() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    unsigned int clilen;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(TEST_PORT);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
         perror("socket binding error");
         exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    clilen = sizeof(cli_addr);
    if ((newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen)) == -1) {
        perror("accept error");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    close(newsockfd);

    pthread_exit(NULL);
}

void mock_handler(int signal) {
    /* Do nothing */
}

void test_connection() {
    pthread_t thread;
    pthread_create(&thread, NULL, mock_server, NULL);

    net_connect("localhost", TEST_PORT);
    mu_assert(_socket > 0, "test_connection: _socket should be > 0");
    mu_assert(_sd != NULL, "test_connection: _sd should not be NULL");

    net_disconnect();
    mu_assert(_socket == 0, "test_connection: _socket should be 0");
    mu_assert(_sd == NULL, "test_connection: _sd should be NULL");
}

void test_send() {
    int socks[2];
    FILE* in;
    char msg[READ_BUF], *ret;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];   /* Override the socket the net_send will use */
    net_send("Outgoing message");

    in = fdopen(socks[1], "r");
    ret = fgets(msg, READ_BUF, in);

    close(socks[0]);
    close(socks[1]);

    mu_assert(ret != NULL, "test_send: fgets should not return NULL");
    mu_assert(s_eq(msg, "Outgoing message\r\n"), "test_send: Message should be 'Outgoing message\\r\\n'");
}

void test_send_longer() {
    int socks[2];
    FILE* in;
    char out[WRITE_BUF + 10], msg[WRITE_BUF + 10], *ret;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];   /* Override the socket the net_send will use */
    memset(out, 'a', WRITE_BUF + 10);
    net_send(out);

    in = fdopen(socks[1], "r");
    ret = fgets(msg, WRITE_BUF + 10, in);

    close(socks[0]);
    close(socks[1]);

    mu_assert(ret != NULL, "test_send: fgets should not return NULL");
    mu_assert(strlen(out) > strlen(msg), "test_send_longer: The received message should be stripped");
    mu_assert(strlen(msg) == (WRITE_BUF + strlen(MSG_SEP)),
            "test_send_longer: Sent message length should be 'WRITE_BUF + strlen(MSG_SEP)'");
}

void test_recv() {
    int socks[2];
    char msg[READ_BUF];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _sd = fdopen(socks[1], "r");   /* Override the socket the net_recv will use */
    send(socks[0], "Outgoing message\r\n", strlen("Outgoing message\r\n"), 0);
    net_recv(msg);

    close(socks[0]);
    close(socks[1]);

    mu_assert(s_eq(msg, "Outgoing message\r\n"), "test_recv: Message should be 'Outgoing message\\r\\n'");
}

void test_recv_longer() {
    int socks[2];
    char in[READ_BUF + 10], out[READ_BUF + 10];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    memset(out, 'a', READ_BUF + 10);
    out[READ_BUF + 9] = '\0';

    _sd = fdopen(socks[1], "r");   /* Override the socket the net_recv will use */
    send(socks[0], out, strlen(out), 0);
    net_recv(in);

    close(socks[0]);
    close(socks[1]);

    mu_assert(strlen(out) > strlen(in), "test_recv_longer: The received message should be stripped");
    mu_assert(strlen(in) == MSG_SIZE, "test_recv_longer: Received message length should be 'MSG_SIZE'");
}

void test_listen_ready() {
    int pid, p2c[2], c2p[2];

    if (pipe(p2c) == -1) {      /* Parent to child pipe */
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    if (pipe(c2p) == -1) {      /* Child to parent pipe */
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid > 0)  { /* Parent process */
        enum net_status status;

        /* Close parent read and client write endpoints */
        close(p2c[0]);
        close(c2p[1]);

        poll(0, 0, 1000); /* Make sure child process is listening */
        if (write(p2c[1], "test", strlen("test")) == -1) {
            perror("write error");
            exit(EXIT_FAILURE);
        }

        /* Read returned value from the child process */
        if (read(c2p[0], &status, sizeof(enum net_status)) == -1) {
            perror("write error");
            exit(EXIT_FAILURE);
        }

        close(p2c[1]);
        close(c2p[0]);

        mu_assert(status == NET_READY, "test_listen_ready: status should be 'NET_READY'");
    } else { /* Child process */
        enum net_status status;

        /* Close parent write and clieht read endpoints */
        close(p2c[1]);
        close(c2p[0]);

        _socket = p2c[0];   /* Overwrite the file descriptor net_listen will use */
        status = net_listen();

        /* Notify the returned value tot he parent process */
        if (write(c2p[1], &status, sizeof(enum net_status)) == -1) {
            perror("write error");
            exit(EXIT_FAILURE);
        }

        close(p2c[0]);
        close(c2p[1]);

        /* Terminate the child process */
        exit(EXIT_SUCCESS);
    }
}

void test_listen_error() {
    int fd[2];
    enum net_status status;

    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    _socket = fd[0];    /* Override the file descriptor net_listen will use */

    close(fd[0]);
    close(fd[1]);

    status = net_listen();

    mu_assert(status == NET_ERROR, "test_listen_error: status should be 'NET_ERROR'");
}

void test_network() {
    mu_run(test_connection);
    mu_run(test_send);
    mu_run(test_send_longer);
    mu_run(test_recv);
    mu_run(test_recv_longer);
    mu_run(test_listen_ready);
    mu_run(test_listen_error);
}

