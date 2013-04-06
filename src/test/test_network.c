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
#include "minunit.h"
#include "test.h"
#include "../lib/utils.h"
#include "../lib/network.c"


void test_connection() {
    net_connect("google.com", 80);  /* TODO: Start a local service to connect to */
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
    int socks[2], pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];   /* Override the socket the net_send will use */
    pid = fork();

    if (pid > 0)  { /* Parent process */
        send(socks[1], (void*) 15, sizeof(int), 0);
    } else { /* Child process */
        enum net_status status = net_listen();

        close(socks[0]);
        close(socks[1]);

        mu_assert(status == NET_READY, "test_listen_ready: status should be 'NET_READY'");
    }
}

void test_listen_error() {
    int socks[2];
    enum net_status status;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];

    close(socks[0]);
    close(socks[1]);

    status = net_listen();

    mu_assert(status == NET_ERROR, "test_listen_error: status should be 'NET_ERROR'");
}

void test_listen_close() {
    int socks[2], pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];   /* Override the socket the net_send will use */
    pid = fork();

    if (pid > 0)  { /* Parent process */
        kill(pid, SIGTERM);
    } else { /* Child process */
        enum net_status status = net_listen();

        close(socks[0]);
        close(socks[1]);

        mu_assert(status == NET_CLOSE, "test_listen_close: status should be 'NET_CLOSE'");
    }
}

void test_network() {
    mu_run(test_connection);
    mu_run(test_send);
    mu_run(test_send_longer);
    mu_run(test_recv);
    mu_run(test_recv_longer);
    mu_run(test_listen_ready);
    mu_run(test_listen_error);
    mu_run(test_listen_close);
    /* test_listen_timeout not needed because NET_TIMEOUT is never
     * going to happen since net_listen does not define a timeout */
}

