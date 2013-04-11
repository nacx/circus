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

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <sys/time.h>

/* Maximum number of parameters in an IRC message */
#define MAX_PARAMS 15

/* Raw IRC message */
struct raw_event {
    struct timeval timestamp;   /* The timestamp when the event was generated */
    char* type;                 /* The IRC message type */
    char* prefix;               /* The message prefix (if any) */
    int   num_params;           /* The number of parameters */
    char* params[MAX_PARAMS];   /* The parameter array */
};

/* ********************************** */
/* User information utility functions */
/* ********************************** */

/* Information of the user who generates an event */
typedef struct {
    char* nick;         /* The nickname of the user */
    char* user;         /* The user name of the user */
    char* server;       /* The server of the user */
} UserInfo;

/* Parse the given user string and build the UserInfo struct */
UserInfo user_info(char* user_ref);

/* ******************* */
/* Generic event types */
/* ******************* */

/* Fired when an error message arrives */
typedef struct {
    struct raw_event* raw;        /* The raw IRC message */
    char* code;                 /* The error code */
    int   num_params;           /* The number of parameters in the message */
    char* params[MAX_PARAMS];   /* The parameters of the message */
    char* message;              /* The error message */
} ErrorEvent;

/* Fired when no specific parsing is defined fot the reveiced event */
typedef struct {
    struct raw_event* raw;        /* The raw IRC message */
    char* code;                 /* The message code */
    int   num_params;           /* The number of parameters in the message */
    char* params[MAX_PARAMS];   /* The parameters of the message */
    char* message;              /* The message */
} GenericEvent;

/* ****************************** */
/* Connection registration events */
/* ****************************** */

/* Fired when the nick is changed */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who generates the event */
    char* new_nick;         /* The new nick for the user */
} NickEvent;

/* Fired when someone quits */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who generates the event */
    char* message;          /* The quit message */
} QuitEvent;

/* ************************ */
/* Channel operation events */
/* ************************ */

/* Fired when a user joins a channel */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who joined a channel */
    char* channel;          /* The channel name */
} JoinEvent;

/* Fired when a user leaves a channel */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who leaved the channel */
    char* channel;          /* The channel name */
    char* message;          /* The part message */
} PartEvent;

/* Fired when someone changes the topic of a channel */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who has changed the topic */
    char* channel;          /* The channel name */
    char* topic;            /* The new topic */
} TopicEvent;

/* Fired when the response to the NAMES arrives */
typedef struct {
    struct raw_event* raw;        /* The raw IRC message */
    int finished;               /* If there are no more users to process (NAMES response is multi-message) */
    char* channel;              /* The channel */
    int num_names;              /* The number of names in the current names list */
    char* names[MAX_PARAMS];    /* The list of users in channel */
} NamesEvent;

/* Fired when the response to the NAMES arrives */
typedef struct {
    struct raw_event* raw;        /* The raw IRC message */
    int finished;               /* If there are no more channels to process (LIST response is multi-message) */
    char* channel;              /* The name of the current channel */
    int num_users;              /* The number of users in the channel */
    char* topic;                /* The current topic in the channel */
} ListEvent;

/* Fired when someone invites to a channel */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who generates the event */
    char* nick;             /* The user being invited to the channel */
    char* channel;          /* The chanel where the user is invited */
} InviteEvent;

/* Fired when someone is kicked in a channel */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user performing the kick */
    char* channel;          /* The channel where the user is kicked from */
    char* nick;             /* The nick of the user being kicked */
    char* message;          /* The kick message */
} KickEvent;

/* Fired when a message is sent to a channel or to a user */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    UserInfo user;          /* The user who sends the message */
    int is_channel;         /* If the message is sent to a channel */
    char* to;               /* The destination of the event (nick or channel) */
    char* message;          /* The text of the message */
} MessageEvent;

/* Fired when someone sets a mode in a channel */
typedef struct {
    struct raw_event* raw;                /* The raw IRC message */
    UserInfo user;                      /* The user who is changing the mode */
    int is_channel;                     /* If the mode applies to a channel or to a user. */
    char* target;                       /* The affected channel or user */
    unsigned short int set_flags;       /* The flags being set */
    unsigned short int unset_flags;     /* The flags being unset */
    char* flag_str;                     /* The textual flag string */
    int num_params;                     /* The number of mode parameters */
    char* params[MAX_PARAMS];           /* The mode parameters (user limit, ban mask, etc) */
} ModeEvent;

/* ******************** */
/* Miscellaneous events */
/* ******************** */

/* Fired when a ping message arrives */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    char* server;           /* Server where the pong response must be sent */
} PingEvent;

/* Fired when a notice arrives */
typedef struct {
    struct raw_event* raw;    /* The raw IRC message */
    char* to;               /* The destination of the message */
    char* text;             /* The text of the message */
} NoticeEvent;

/* ************************ */
/* Event building functions */ 
/* ************************ */

ErrorEvent      evt_error(struct raw_event *raw);
GenericEvent    evt_generic(struct raw_event *raw);
NickEvent       evt_nick(struct raw_event *raw);
QuitEvent       evt_quit(struct raw_event *raw);
JoinEvent       evt_join(struct raw_event *raw);
PartEvent       evt_part(struct raw_event *raw);
TopicEvent      evt_topic(struct raw_event *raw);
NamesEvent      evt_names(struct raw_event *raw);
ListEvent       evt_list(struct raw_event *raw);
InviteEvent     evt_invite(struct raw_event *raw);
KickEvent       evt_kick(struct raw_event *raw);
MessageEvent    evt_message(struct raw_event *raw);
ModeEvent       evt_mode(struct raw_event *raw);
PingEvent       evt_ping(struct raw_event *raw);
NoticeEvent     evt_notice(struct raw_event *raw);

/* ************** */
/* Callback types */
/* ************** */

/* Generic callback pointer */
typedef void (*CallbackPtr)(void);

/* Macros to convert to the appropriate function pointer type for each event */
#define ErrorCallback(callback) ((void (*)(ErrorEvent*)) callback)
#define GenericCallback(callback) ((void (*)(GenericEvent*)) callback)
#define NickCallback(callback) ((void (*)(NickEvent*)) callback)
#define QuitCallback(callback) ((void (*)(QuitEvent*)) callback)
#define JoinCallback(callback) ((void (*)(JoinEvent*)) callback)
#define PartCallback(callback) ((void (*)(PartEvent*)) callback)
#define TopicCallback(callback) ((void (*)(TopicEvent*)) callback)
#define NamesCallback(callback) ((void (*)(NamesEvent*)) callback)
#define ListCallback(callback) ((void (*)(ListEvent*)) callback)
#define InviteCallback(callback) ((void (*)(InviteEvent*)) callback)
#define KickCallback(callback) ((void (*)(KickEvent*)) callback)
#define MessageCallback(callback) ((void (*)(MessageEvent*)) callback)
#define ModeCallback(callback) ((void (*)(ModeEvent*)) callback)
#define NoticeCallback(callback) ((void (*)(NoticeEvent*)) callback)
#define PingCallback(callback) ((void (*)(PingEvent*)) callback)

#endif

