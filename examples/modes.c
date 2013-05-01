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
 * This example shows how to change channel and user modes.
 * When the bot is given operator privileges, it changes a set
 * of modes, and thanks the user who granted the operator privileges.
 */

#define _POSIX_C_SOURCE 200112L      /* Use snprintf */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"                  /* Utility functions and macros */
#include "irc.h"                    /* IRC protocol functions */

#define CONF_NICK "circus-bot"      /* The nick to be used by the bot */
#define CONF_CHAN "#circus-bot"     /* The channel to join */


/* Print the error message */
void on_error(ErrorEvent* event) {
    printf("Incoming error message: [%s] %s\n", event->code, event->message);
}

/* Thank operator access and change some modes */
void on_mode(ModeEvent* event) {
    char msg[30];
    int i;

    if (event->set_flags & CH_OPERATOR) {   /* The operator flag is set */
        for (i = 0; i < event->num_params; i++) {
            if (s_eq(event->params[i], CONF_NICK)) { /* We've been given operator privilege */
                snprintf(msg, 30, "Thanks %s!", event->user.nick);  /* Build the message to send */
                irc_message(event->target, msg);
        
                /* Channel flags are defined in irc.h */
                irc_channel_set(event->target, CH_INVITEONLY | CH_MODERATED);
                irc_channel_unset(event->target, CH_INVITEONLY | CH_MODERATED);

                irc_ban(event->target, "someone!*@*");
                irc_ban_list(event->target);
                irc_unban(event->target, "someone!*@*");

                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    char* server, *port;

    if (argc != 3) {
        printf("Usage: %s <server> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server = argv[1];   /* The IRC server */
    port = argv[2];     /* The IRC server port */

    /* Bind IRC event to custom functions.
     * All bindable events are defined in codes.h */
    irc_bind_event(ERROR, (Callback) on_error);
    irc_bind_event(MODE, (Callback) on_mode);

    /* Connect, login and join the configured channel */
    irc_connect(server, port);
    irc_login(CONF_NICK, "Circus", "Circus IRC bot");
    irc_join(CONF_CHAN);

    /* Set/unset some user flags just to show how to manipulate them.
     * All flags are defined in irc.h */
    irc_user_set(CONF_NICK, USR_WALLOPS | USR_INVISIBLE);
    irc_user_unset(CONF_NICK, USR_WALLOPS);

    /* Start listening to events.
     * This method blocks until a quit signal is received */
    irc_listen();

    /* Send quit message and close connection */
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}

