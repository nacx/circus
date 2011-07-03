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
 * Read welcome.c, oper.c and binding.c exampes first.
 *
 * This first example shows generic callback features to handle error
 * responses and to define global message handlers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"                  // Utility functions and macros
#include "events.h"                 // Event types for callback functions
#include "irc.h"                    // IRC protocol functions


#define CONF_NICK "circus-bot"      // The nick to be used by the bot
#define CONF_CHAN "#circus-bot"     // The channel to join

// Disconnect if the nick is in use
void on_nick_in_use(ErrorEvent* event) {
    printf("Nick %s is already in use\n", event->params[1]);
    irc_quit("Bye");
    irc_disconnect();
    exit(EXIT_FAILURE);
}

// Print the error message
void on_error(ErrorEvent* event) {
    printf("Error message arrived: [%s] %s\n", event->code, event->message);
}

// Welcome a user when joining
void on_join(JoinEvent* event) {
    char msg[30];
    if (s_ne(event->user.nick, CONF_NICK)) {                // String not-equal macro from utils.h
        snprintf(msg, 30, "Welcome %s", event->user.nick);  // Build the message to send
        irc_channel(event->channel, msg);                   // Send message to channel
    }
}

// Print the incoming message
void on_message(GenericEvent* event) {
    printf("Incoming message: [%s] %s\n", event->code, event->message);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server = argv[1];     // The IRC server
    int port = atoi(argv[2]);   // The IRC server port

    // Bind IRC event to custom functions
    irc_bind_event(ERR_NICKNAMEINUSE, on_nick_in_use);
    irc_bind_event(ERROR, on_error);
    irc_bind_event(JOIN, on_join);
    irc_bind_event(ALL, on_message);

    // Connect, login and join the configured channel
    irc_connect(server, port);
    irc_login(CONF_NICK, "Circus", "Circus IRC bot");
    irc_join(CONF_CHAN);

    // Start listening to events
    // This method blocks until a quit signal is received
    irc_listen();

    // Send quit message and close connection
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}
