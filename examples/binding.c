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
 * Read welcome.c and oper.c examples first.
 *
 * This is an example bot that shows Circus event binding features.
 * It defines two control methods to enable and disable the defined
 * callbacks on the fly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"                  // Utility functions and macros
#include "events.h"                 // Event types for callback functions
#include "irc.h"                    // IRC protocol functions


#define CONF_NICK "circus-bot"      // The nick to be used by the bot
#define CONF_CHAN "#circus-bot"     // The channel to join


// Welcome a user when joining the channel
void welcome(JoinEvent* event) {
    char msg[30];
    if (s_ne(event->user.nick, CONF_NICK)) {                // String not-equal macro from utils.h
        snprintf(msg, 30, "Welcome %s", event->user.nick);  // Build the message to send
        irc_channel(event->channel, msg);                   // Send message to channel
    }
}

// Give op to the user who has requested it
void give_op(MessageEvent* event) {
    irc_op(event->to, event->user.nick);
}

// Disables bot callbacks
void disable(MessageEvent* event) {
    irc_unbind_event(JOIN);
    irc_unbind_command("!op");
}

// Enables bot callbacks
void enable(MessageEvent* event) {
    irc_bind_event(JOIN, welcome);
    irc_bind_command("!op", give_op);
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server = argv[1];     // The IRC server
    int port = atoi(argv[2]);   // The IRC server port

    // Bind IRC events and message commands to custom functions
    irc_bind_event(JOIN, welcome);
    irc_bind_command("!disable", disable);
    irc_bind_command("!enable", enable);
    irc_bind_command("!op", give_op);

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

