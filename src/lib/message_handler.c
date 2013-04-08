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

#define _XOPEN_SOURCE   /* Support strtok_r in POSIX and BSD environments */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "events.h"
#include "utils.h"
#include "binding.h"
#include "codes.h"
#include "irc.h"
#include "debug.h"
#include "message_handler.h"

/* *************** */
/* Utility methods */
/* *************** */

static struct raw_msg new_raw_message() {
    int i;
    struct raw_msg raw;

    gettimeofday(&raw.timestamp, NULL);
    raw.prefix = NULL;
    raw.type = NULL;
    raw.num_params = 0;

    for (i = 0; i < MAX_PARAMS; i++) {
        raw.params[i] = NULL;
    }

    return raw;
}

/* ********************* */
/* System event handlers */
/* ********************* */

static void __circus__ping_handler(PingEvent* event) {
    irc_pong(event->server);
}

/* **************** */
/* Event triggering */
/* **************** */

void fire_event(struct raw_msg *raw) {
    CallbackPtr callback = NULL;
    upper(raw->type);

    /* Check if there is a concrete binding for the
     * incoming message type. */
    debug(("handler: Looking for a binding for %s\n", raw->type));

    /* Connection registration */
    if (s_eq(raw->type, NICK)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            NickEvent event = nick_event(raw);
            NickCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, QUIT)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            QuitEvent event = quit_event(raw);
            QuitCallback(callback)(&event);
        }
    } /* Channel operations */
    else if (s_eq(raw->type, JOIN)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            JoinEvent event = join_event(raw);
            JoinCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, PART)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            PartEvent event = part_event(raw);
            PartCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, TOPIC)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            TopicEvent event = topic_event(raw);
            TopicCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, RPL_NAMREPLY) || s_eq(raw->type, RPL_ENDOFNAMES)) {
        callback = lookup_event(NAMES);
        if (callback != NULL) {
            NamesEvent event = names_event(raw);
            NamesCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, RPL_LIST) || s_eq(raw->type, RPL_LISTEND)) {
        callback = lookup_event(LIST);
        if (callback != NULL) {
            ListEvent event = list_event(raw);
            ListCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, INVITE)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            InviteEvent event = invite_event(raw);
            InviteCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, KICK)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            KickEvent event = kick_event(raw);
            KickCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, PRIVMSG)) {
        /* Look for a command binding */
        char key[50];
        char* buffer, *command, *command_params;
        size_t lparam = strlen(raw->params[1]);

        /* The first parameter in PRIVMSG contains the whole message.
         * We need to consider only the first word */
        if ((buffer = malloc((lparam + 1) * sizeof(char))) == 0) {
            perror("Out of memory (fire_event)");
            exit(EXIT_FAILURE);
        }

        memset(buffer, '\0', lparam + 1);
        strncpy(buffer, raw->params[1], lparam);
        command = strtok_r(buffer, " ", &command_params);

        if (command != NULL) {
            build_command_key(key, command);
            debug(("handler: Looking for command: %s\n", command));
            callback = lookup_event(key);
            if (callback != NULL) {
                /* Remove the command name from the raw message */
                raw->params[1] = command_params;
            }
        }

        /* If no command binding is found, look for an event binding */
        if (callback == NULL) {
            debug(("handler: No command found. Looking for event.\n"));
            callback = lookup_event(raw->type);
        }

        if (callback != NULL) {
            MessageEvent event = message_event(raw);
            MessageCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, MODE)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            ModeEvent event = mode_event(raw);
            ModeCallback(callback)(&event);
        }
    } /* Miscellaneous events */
    else if (s_eq(raw->type, PING)) {
        PingEvent event = ping_event(raw);
        __circus__ping_handler(&event);    /* Call the system callback for ping before calling the bindings */
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            PingCallback(callback)(&event);
        }
    } else if (s_eq(raw->type, NOTICE)) {
        callback = lookup_event(raw->type);
        if (callback != NULL) {
            NoticeEvent event = notice_event(raw);
            NoticeCallback(callback)(&event);
        }
    }

    /* If no specific callback is found, check if there is
     * a global binding defined to handle the incoming message. */

    if (callback == NULL) {
        if (is_error(raw->type)) {
            /* Look for a concrete error binding */
            callback = lookup_event(raw->type);
            /* If none is found, look for a generic error binding */
            if (callback == NULL) {
                callback = lookup_event(ERROR);
            }

            if (callback != NULL) {
                ErrorEvent event = error_event(raw);
                ErrorCallback(callback)(&event);
            }
        } else {
            /* Look for a concrete message binding */
            callback = lookup_event(raw->type);
            /* If none is found, look for a generic message binding */
            if (callback == NULL) {
                callback = lookup_event(ALL);
            }

            if (callback != NULL) {
                GenericEvent event = generic_event(raw);
                GenericCallback(callback)(&event);
            }
        }
    }

    debug(("handler: Binding%sfound\n", callback == NULL? " not " : " "));
}

/* *************** */
/* Message parsing */
/* *************** */

struct raw_msg parse(char* msg, char* buffer) {
    int i = 0, is_last_parameter = 0;
    size_t msg_len;
    struct raw_msg raw = new_raw_message();
    char *token, *token_end = NULL;

    if (msg != NULL && (msg_len = strlen(msg)) > 0) {

        if ((buffer = malloc((msg_len + 1) * sizeof(char))) == 0) {
            perror("Out of memory (parse)");
            exit(EXIT_FAILURE);
        }

        memset(buffer, '\0', msg_len + 1);
        strncpy(buffer, msg, msg_len);

        token = strtok_r(buffer, PARAM_SEP, &token_end);
        while(token != NULL)
        {
            if (raw.type == NULL) {
                /* If type is not set, we must check if there
                 * is a message prefix. */
                if (token[0] == ':') {
                    raw.prefix = token + 1; /* Ignore the ':' */
                } else {
                    raw.type = token;
                }
            } else { /* If the type is set the token is a parameter */
                /* If a parameter begins with ':' then it is
                 * the last parameter and it is all the remaining message. */
                if (token[0] == ':') {
                    /* Do not increment the parameter count */
                    raw.params[i] = token + 1; /* Ignore the ':' */
                    is_last_parameter = 1;       /* Set the last parameter flag */
                } else {
                    if (is_last_parameter == 0) {
                        raw.params[i++] = token;
                    } else {
                        /* If it is the last parameter, just concatenate
                         * the tokens. */
                        sprintf(raw.params[i], "%s %s", raw.params[i], token);
                    }
                }
            }

            /* Continue with the next token */
            token = strtok_r(NULL, PARAM_SEP, &token_end);
        }

        /* If we have a last parameter we should increment now the parameter
         * counter. */
        raw.num_params = (is_last_parameter == 0)? i : i + 1;
    }

    return raw;
}

void handle(char* msg) {
    char* buffer = NULL;
    struct raw_msg raw;

    msg[strlen(msg) - 2] = '\0';    /* Remove the line terminaion before parsing */
    raw = parse(msg, buffer);       /* Parse the input and get the raw message */
    fire_event(&raw);               /* Fire the event */

    /* Free memory used to parse the message once it has been handled */
    free(buffer);
}

