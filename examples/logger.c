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
 * Read welcome.c and oper.c first.
 *
 * This is an example logging bot that logs conversations in all channels where
 * the bot is connected and all private messages sent to it.
 *
 * It also defines a binding to disconnect if the given nick is already in use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "irc.h"                    /* IRC protocol functions */

/* The location of the log files */
#define LOG_PATH "/tmp/circus"


/* Disconnect if the nick is in use */
void on_nick_in_use(ErrorEvent* event) {
    printf("Nick %s is already in use\n", event->params[1]);
    irc_quit("Bye");
    irc_disconnect();
    exit(EXIT_FAILURE);
}

/* Log message to the log file */
void log_msg(MessageEvent* event) {
    char log_file[30], str_time[64];
    FILE* f;
    struct tm* event_time;

    /* Build the file where the log will be appended to */
    sprintf(log_file, "%s/%s", LOG_PATH, event->is_channel? event->to : event->user.nick);

    /* Pretty format the event timestamp */
    event_time = localtime(&event->timestamp->tv_sec);
    strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M:%S", event_time);

    /* Write the log */
    f = fopen(log_file, "a");
    fprintf(f, "[%s] <%s> %s\n", str_time, event->user.nick, event->message);
    fclose(f);
}


int main(int argc, char **argv) {
    int i;
    char *server, *port, *nick;

    if (argc < 4) {
        printf("Usage: %s <server> <port> <nick> ['<channel 1>' ... '<channel n>']\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server = argv[1];   /* The IRC server */
    port = argv[2];     /* The IRC server port */
    nick = argv[3];     /* The nick to use */

    /* Create the log directory */
    mkdir(LOG_PATH, S_IRWXU);

    /* Bind IRC event to custom functions.
     * All bindable events are defined in codes.h */
    irc_bind_event(ERR_NICKNAMEINUSE, (Callback) on_nick_in_use);
    irc_bind_event(PRIVMSG, (Callback) log_msg);

    /* Connect, login and join the configured channel */
    irc_connect(server, port);
    irc_login(nick, "Circus", "Circus IRC bot");

    /* Join the given channels */
    for (i = 4; i < argc; i++) {
        irc_join(argv[i]);
    }

    /* Start listening to events.
     * This method blocks until a quit signal is received */
    irc_listen();

    /* Send quit message and close connection */
    irc_quit("Bye");
    irc_disconnect();

    return 0;
}

