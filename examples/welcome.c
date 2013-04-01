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
 * This is an example bot that welcomes the users when joining the
 * channels where the bot is, and says goodbye in a private message
 * when a user leaves.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"                  // Utility functions and macros
#include "irc.h"                    // IRC protocol functions

#define CONF_NICK "circus-bot"      // The nick to be used by the bot
#define CONF_CHAN "#circus-bot"     // The channel to join

/*
 * Welcomes a user when joining the channel.
 * If the user is the bot itself, do nothing.
 */
void on_join(JoinEvent* event) {    // Events details are defined in events.h
    char msg[30];
    if (s_ne(event->user.nick, CONF_NICK)) {                // String not-equal macro from utils.h
        snprintf(msg, 30, "Welcome %s", event->user.nick);  // Build the message to send
        irc_channel_msg(event->channel, msg);               // Send message to channel
    }
}

/*
 * Says goodbye to a user in a private message when leaving the channel.
 * If the user is the bot itself, do nothing.
 */
void on_part(PartEvent* event) {    // Event details are defined in events.h
    char msg[30];
    if (s_ne(event->user.nick, CONF_NICK)) {                // String not-equal macro from utils.h
        snprintf(msg, 30, "Good bye %s", event->user.nick); // Build the message to send
        irc_private_msg(event->user.nick, msg);             // Send the private message to the user
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server = argv[1];     // The IRC server
    int port = atoi(argv[2]);   // The IRC server port

    // Bind IRC event to custom functions
    // All bindable events are defined in codes.h
    irc_bind_event(JOIN, (CallbackPtr) on_join);
    irc_bind_event(PART, (CallbackPtr) on_part);

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

