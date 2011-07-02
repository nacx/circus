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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "events.h"
#include "irc.h"

#define CONF_NICK "circus-bot"
#define CONF_CHAN "#circus-bot"


void on_join(JoinEvent* event) {
    char msg[30];
    if (s_ne(event->nick, CONF_NICK)) {
        snprintf(msg, 30, "Welcome %s", event->nick);
        irc_channel(event->channel, msg);
    }
}

void on_part(PartEvent* event) {
    char msg[30];
    if (s_ne(event->nick, CONF_NICK)) {
        snprintf(msg, 30, "Good bye %s", event->nick);
        irc_private(event->nick, msg);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server = argv[1];
    int port = atoi(argv[2]);

    // Bind IRC event to custom functions
    irc_bind(JOIN, on_join);
    irc_bind(PART, on_part);

    // Connect and start listening to events
    irc_connect(server, port);
    irc_login(CONF_NICK, "Circus", "Circus IRC bot");
    irc_join(CONF_CHAN);

    // This method blocks until a quit signal is received
    irc_listen();

    // Quit from IRC and close connection
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}

