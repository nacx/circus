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
#include "utils.h"
#include "events.h"
#include "message_handler.h"
#include "irc.h"


/* ********************************** */
/* User information utility functions */
/* ********************************** */

UserInfo user_info(char* user_ref) {
    UserInfo ui;
    char* c;

    if (user_ref == NULL) {
        ui.nick = NULL;
        ui.user = NULL;
        ui.server = NULL;
    } else {
        c = user_ref;
        ui.nick = c;

        while (*c != '!') c++;
        *c = '\0';
        ui.user = c + 2;

        while (*c != '@') c++;
        *c = '\0';
        ui.server = c + 1;
    }

    return ui;
}

/* ************** */
/* Generic events */
/* ************** */

ErrorEvent error_event(struct raw_msg *raw) {
    int i;
    ErrorEvent event;

    for (i = 0; i < raw->num_params - 1; i++) {
        event.params[i] = raw->params[i];
    }

    event.code = raw->type;
    event.num_params = i;
    event.message = raw->params[raw->num_params - 1];

    return event;
}

GenericEvent generic_event(struct raw_msg *raw) {
    int i;
    GenericEvent event;

    for (i = 0; i < raw->num_params - 1; i++) {
        event.params[i] = raw->params[i];
    }

    event.code = raw->type;
    event.num_params = i;
    event.message = raw->params[raw->num_params - 1];

    return event;
}

/* ****************************** */
/* Connection registration events */
/* ****************************** */

NickEvent nick_event(struct raw_msg *raw) {
    NickEvent event;
    event.user = user_info(raw->prefix);
    event.new_nick = raw->params[0];
    return event;
}

QuitEvent quit_event(struct raw_msg *raw) {
    QuitEvent event;
    event.user = user_info(raw->prefix);
    event.message = raw->params[0];
    return event;
}

/* ************************ */
/* Channel operation events */
/* ************************ */

JoinEvent join_event(struct raw_msg *raw) {
    JoinEvent event;
    event.user = user_info(raw->prefix);
    event.channel = raw->params[0];
    return event;
}

PartEvent part_event(struct raw_msg *raw) {
    PartEvent event;
    event.user = user_info(raw->prefix);
    event.channel = raw->params[0];
    event.message = raw->params[1];
    return event;
}

TopicEvent topic_event(struct raw_msg *raw) {
    TopicEvent event;
    event.user = user_info(raw->prefix);
    event.channel = raw->params[0];
    event.topic = raw->params[1];
    return event;
}

NamesEvent names_event(struct raw_msg *raw) {
    char* token, *next = NULL;
    NamesEvent event;
    event.finished = s_eq(raw->type, RPL_ENDOFNAMES);
    event.channel = raw->params[event.finished? 1 : 2];
    event.num_names = 0;
    if (!event.finished) {
        token = strtok_r(raw->params[3], " ", &next);
        while(token != NULL) {
            event.names[event.num_names++] = token;
            token = strtok_r(NULL, " ", &next);
        }
    }
    return event;
}

ListEvent list_event(struct raw_msg *raw) {
    ListEvent event;
    event.channel = NULL;
    event.num_users = 0;
    event.topic = NULL;
    event.finished = s_eq(raw->type, RPL_LISTEND);
    if (!event.finished) {
        event.channel = raw->params[1];
        event.num_users = atoi(raw->params[2]);
        event.topic = raw->params[3];
    }
    return event;
}

InviteEvent invite_event(struct raw_msg *raw) {
    InviteEvent event;
    event.user = user_info(raw->prefix);
    event.nick = raw->params[0];
    event.channel = raw->params[1];
    return event;
}

KickEvent kick_event(struct raw_msg *raw) {
    KickEvent event;
    event.user = user_info(raw->prefix);
    event.channel = raw->params[0];
    event.nick = raw->params[1];
    event.message = raw->params[2];
    return event;
}

MessageEvent message_event(struct raw_msg *raw) {
    MessageEvent event;
    event.user = user_info(raw->prefix);
    event.is_channel = (raw->params[0][0] == '#');
    event.to = raw->params[0];
    event.message = raw->params[1];
    return event;
}

ModeEvent mode_event(struct raw_msg *raw) {
    ModeEvent event;
    int i;
    char op = '\0';  /* Invalid character to initialize variable */
    unsigned short int current_flag = 0x0000;
    char* flags = raw->params[1];

    event.is_channel = (raw->params[0][0] == '#');
    event.user = user_info(event.is_channel? raw->prefix : NULL);
    event.target = raw->params[0];
    event.set_flags = 0x0000;
    event.unset_flags = 0x0000;
    event.flag_str = flags;
    event.num_params = raw->num_params - 2;

    /* Process flags */
    for (i = 0; i < strlen(flags); i++) {
        if (flags[i] == '+' || flags[i] == '-') {
            op = flags[i];
            continue;
        }

        switch (flags[i]) {
            case 'p': current_flag = CH_PRIVATE;    break;
            case 't': current_flag = CH_TOPICLOCK;  break;
            case 'n': current_flag = CH_NOEXTMSGS;  break;
            case 'm': current_flag = CH_MODERATED;  break;
            case 'b': current_flag = CH_BAN;        break;
            case 'l': current_flag = CH_LIMIT;      break;
            case 'v': current_flag = CH_VOICE;      break;
            case 'k': current_flag = CH_KEY;        break;
            case 's': current_flag = CH_SECRET;     break;
            case 'w': current_flag = USR_WALLOPS;   break;
            case 'i': current_flag = event.is_channel? CH_INVITEONLY : USR_INVISIBLE;       break;
            case 'o': current_flag = event.is_channel? CH_OPERATOR   : USR_OPERATOR;        break;
            default:  current_flag = 0x0000; break;
        }

        /* Update the corresponding flags */
        if (op == '+') {
            event.set_flags |= current_flag;
        } else if (op == '-') {
            event.unset_flags |= current_flag;
        }
    }

    /* Add the mode parameters (only available in channel mode) */
    if (event.num_params > 0) {
        for (i = 0; i < event.num_params; i++) {
            event.params[i] = raw->params[i + 2];
        }
    }

    return event;
}


/* ******************** */
/* Miscellaneous events */
/* ******************** */

PingEvent ping_event(struct raw_msg *raw) {
    PingEvent event;
    event.server = raw->params[0];
    return event;
}

NoticeEvent notice_event(struct raw_msg *raw) {
    NoticeEvent event;
    event.to = raw->params[0];
    event.text = raw->params[1];
    return event;
}

