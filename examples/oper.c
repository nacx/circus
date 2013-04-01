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
 * This is an example bot that give operator and voice mode when
 * requested.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irc.h"                    /* IRC protocol functions */

#define CONF_NICK "circus-bot"      /* The nick to be used by the bot */
#define CONF_CHAN "#circus-bot"     /* The channel to join */

/* Give op to the user who has requested it */
void give_op(MessageEvent* event) {     /* Event details are defined in events.h */
    /* The event->to variable is the channel where the message comes from, */
    /* if the message was sent to a channel, or the sender's nick, if the */
    /* message was a private message */
    irc_op(event->to, event->user.nick);
}

/* Take op to the user who has requested it */
void take_op(MessageEvent* event) {     /* Event details are defined in events.h */
    irc_deop(event->to, event->user.nick);
}

/* Give voice to the user who has requested it */
void give_voice(MessageEvent* event) {  /* Event details are defined in events.h */
    irc_voice(event->to, event->user.nick);
}

/* Take voice to the user who has requested it */
void take_voice(MessageEvent* event) {  /* Event details are defined in events.h */
    irc_devoice(event->to, event->user.nick);
}

int main(int argc, char **argv) {
    char* server;
    int port;

    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server = argv[1];     /* The IRC server */
    port = atoi(argv[2]);   /* The IRC server port */

    /* Bind message commands to custom functions */
    irc_bind_command("!op", (CallbackPtr) give_op);
    irc_bind_command("!deop", (CallbackPtr) take_op);
    irc_bind_command("!voice", (CallbackPtr) give_voice);
    irc_bind_command("!devoice", (CallbackPtr) take_voice);

    /* Connect, login and join the configured channel */
    irc_connect(server, port);
    irc_login(CONF_NICK, "Circus", "Circus IRC bot");
    irc_join(CONF_CHAN);

    /* Start listening to events */
    /* This method blocks until a quit signal is received */
    irc_listen();

    /* Send quit message and close connection */
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}

