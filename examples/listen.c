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

/*
 * This example is a dummy bot that simply connects to the IRC and begins to listen
 * for incoming messages.
 * It only defines a binding to disconnect if the given nick is already in use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irc.h"                    // IRC protocol functions


// Disconnect if the nick is in use
void on_nick_in_use(ErrorEvent* event) {
    printf("Nick %s is already in use\n", event->params[1]);
    irc_quit("Bye");
    irc_disconnect();
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Usage: %s <server> <port> <nick> <channel>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server = argv[1];     // The IRC server
    int port = atoi(argv[2]);   // The IRC server port
    char* nick = argv[3];       // The nick to use
    char* channel = argv[4];    // The channel to connect to

    // Bind IRC event to custom functions
    irc_bind_event(ERR_NICKNAMEINUSE, on_nick_in_use);

    // Connect, login and join the configured channel
    irc_connect(server, port);
    irc_login(nick, "Circus", "Circus IRC bot");
    irc_join(channel);

    // Start listening to events
    // This method blocks until a quit signal is received
    irc_listen();

    // Send quit message and close connection
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}

