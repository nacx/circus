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
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "network.h"
#include "message_handler.h"
#include "binding.h"
#include "irc.h"

// Flag used to close the connection
int shutdown_requested = 0;


/* ******************** */
/* Connection functions */
/* ******************** */

void irc_connect(char* address, int port) {
    net_connect(address, port);
}

void irc_disconnect() {
    net_disconnect();
}

void shutdown_handler(int signal) {
    switch (signal) {
        case SIGHUP:
        case SIGTERM:
        case SIGINT:
            shutdown_requested = 1;
            break;
        default:
            break;
    }
}

void irc_listen() {
    int status;
    char msg[MSG_SIZE];

    // Register shutdown signals
    signal(SIGHUP, shutdown_handler);
    signal(SIGTERM, shutdown_handler);
    signal(SIGINT, shutdown_handler);

    while (shutdown_requested == 0) {
        status = net_listen();

        switch(status) {
            case NET_ERROR:
                perror("Error listening for messages");
                exit(EXIT_FAILURE);
                break;
            case NET_CLOSE:
                // The shutdown flag should have been set by
                // the signal handler, but it is safe to set it again
                shutdown_requested = 1;
                break;
            case NET_READY:
                net_recv(msg);
                handle(msg);
                break;
            case NET_IGNORE:
                // Do nothing. The loop should continue
                break;
            default:
                break;
        }
    }
}

/* *********************** */
/* Event binding functions */
/* *********************** */

void irc_bind(char* event, void* callback) {
    bind_event(event, callback);
}

void irc_unbind(char* event) {
    unbind_event(event);
}

/* **************** */
/* System functions */
/* **************** */

void irc_pong(char* server) {
    char msg[MSG_SIZE];
    snprintf(msg, MSG_SIZE, "PONG %s", server);
    net_send(msg);
}

/* ************** */
/* User functions */
/* ************** */

void irc_nick(char* nick) {
    char msg[MSG_SIZE];
    snprintf(msg, MSG_SIZE, "NICK %s", nick);
    net_send(msg);
}

void irc_user(char* user_name, char* real_name) {
    char msg[MSG_SIZE];
    snprintf(msg, MSG_SIZE, "USER %s hostname server :%s", user_name, real_name);
    net_send(msg);
}

void irc_login(char* nick, char* user_name, char* real_name) {
    irc_nick(nick);
    irc_user(user_name, real_name);
}

void irc_quit(char* quit_msg) {
    char msg[MSG_SIZE];
    snprintf(msg, MSG_SIZE, "QUIT :%s", quit_msg);
    net_send(msg);
}

