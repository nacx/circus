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
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include "network.h"
#include "message_handler.h"
#include "binding.h"
#include "utils.h"
#include "irc.h"

// Flag used to close the connection
static int shutdown_requested = 0;


/* *********************** */
/* Event binding functions */
/* *********************** */

void irc_bind_event(char* event, void* callback) {
    bind_event(event, callback);
}

void irc_unbind_event(char* event) {
    unbind_event(event);
}

void irc_bind_command(char* command, void* callback) {
    char* key;

    if ((key = malloc(50 * sizeof(char))) == 0) {
        perror("Out of memory (irc_bind_command)");
        exit(EXIT_FAILURE);
    }

    memset(key, '\0', 50);
    build_command_key(key, command);
    
    bind_event(key, callback);
}

void irc_unbind_command(char* command) {
    char* key, *ret;

    if ((key = malloc(50 * sizeof(char))) == 0) {
        perror("Out of memory (irc_unbind_command)");
        exit(EXIT_FAILURE);
    }
    
    memset(key, '\0', 50);
    build_command_key(key, command);
    ret = unbind_event(key);

    free(key);  // Free the memory allocated un this function
    if (ret != NULL) {
        free(ret);  // Free the memory allocated when binding the command
    }
}

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
            cleanup_bindings();
            break;
        default:
            break;
    }
}

void irc_listen() {
    enum net_status status;
    char msg[READ_BUF];

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

void irc_nick(char* nick) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s", NICK, nick);
    net_send(msg);
}

void irc_user(char* user_name, char* real_name) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s hostname server :%s", USER, user_name, real_name);
    net_send(msg);
}

void irc_login(char* nick, char* user_name, char* real_name) {
    irc_nick(nick);
    irc_user(user_name, real_name);
}

void irc_quit(char* message) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s :%s", QUIT, message);
    net_send(msg);
}

/* ****************** */
/* Channel operations */
/* ****************** */

void irc_join(char* channel) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s", JOIN, channel);
    net_send(msg);
}

void irc_join_pass(char* channel, char* pass) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s %s", JOIN, channel, pass);
    net_send(msg);
}

void irc_part(char* channel) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s", PART, channel);
    net_send(msg);
}

void irc_topic(char* channel, char* topic) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s :%s", TOPIC, channel, topic);
    net_send(msg);
}

void irc_names(char* channel) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s", NAMES, channel);
    net_send(msg);
}

void irc_list() {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s", LIST);
    net_send(msg);
}

void irc_invite(char* nick, char* channel) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s %s", INVITE, nick, channel);
    net_send(msg);
}

void irc_channel_msg(char* channel, char* message) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s :%s", PRIVMSG, channel, message);
    net_send(msg);
}

void irc_private_msg(char* nick, char* message) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s :%s", PRIVMSG, nick, message);
    net_send(msg);
}

void irc_op(char* channel, char* nick) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +o %s", MODE, channel, nick);
    net_send(msg);
}

void irc_deop(char* channel, char* nick) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s -o %s", MODE, channel, nick);
    net_send(msg);
}

void irc_voice(char* channel, char* nick) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +v %s", MODE, channel, nick);
    net_send(msg);
}

void irc_devoice(char* channel, char* nick) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s -v %s", MODE, channel, nick);
    net_send(msg);
}

void irc_kick(char* channel, char* nick, char* message) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s %s :%s", KICK, channel, nick, message);
    net_send(msg);
}

void irc_ban(char* channel, char* mask) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +b %s", MODE, channel, mask);
    net_send(msg);
}

void irc_unban(char* channel, char* mask) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s -b %s", MODE, channel, mask);
    net_send(msg);
}

void irc_ban_list(char* channel) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +b", MODE, channel);
    net_send(msg);
}

void irc_limit(char* channel, int limit) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +l %d", MODE, channel, limit);
    net_send(msg);
}

void irc_channel_key(char* channel, char* key) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +k %s", MODE, channel, key);
    net_send(msg);
}

void irc_channel_set(char* channel, unsigned short int flags) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +", MODE, channel);
    append_channel_flags(msg, flags);
    net_send(msg);
}

void irc_channel_unset(char* channel, unsigned short int flags) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s -", MODE, channel);
    append_channel_flags(msg, flags);
    net_send(msg);
}

/* *************** */
/* User operations */
/* *************** */

void irc_user_set(char* user, unsigned short int flags) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s +", MODE, user);
    append_user_flags(msg, flags);
    net_send(msg);
}

void irc_user_unset(char* user, unsigned short int flags) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s -", MODE, user);
    append_user_flags(msg, flags);
    net_send(msg);
}

/* *********************** */
/* Miscellaneous functions */
/* *********************** */

void irc_pong(char* server) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, "%s %s", PONG, server);
    net_send(msg);
}

/* **************** */
/* System functions */
/* **************** */

void irc_raw(char* prefix, char* type, char* message) {
    char msg[WRITE_BUF];
    snprintf(msg, WRITE_BUF, ":%s %s %s", prefix, type, message);
    net_send(msg);
}

