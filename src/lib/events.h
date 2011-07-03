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

// Maximum number of parameters in an IRC message
#define MAX_PARAMS 15

// Raw IRC message
struct raw_msg {
    char* type;                 // The IRC message type
    char* prefix;               // The message prefix (if any)
    int   num_params;           // The number of parameters
    char* params[MAX_PARAMS];   // The parameter array
};

/* ********************************** */
/* User information utility functions */
/* ********************************** */

// Information of the user who generates an event
typedef struct {
    char* nick;
    char* user;
    char* server;
} UserInfo;

// Parse the given user string and build the UserInfo struct
UserInfo user_info(char* user_ref);

/* *************** */
/* IRC Event types */
/* *************** */

// Fired when an error message arrives
typedef struct {
    char* code;
    int   num_params;
    char* params[MAX_PARAMS];
    char* message;
} ErrorEvent;

// Fired when no specific parsing is defined fot the reveiced event
typedef struct {
    char* code;
    int   num_params;
    char* params[MAX_PARAMS];
    char* message;
} GenericEvent;

// Fired when a ping message arrives
typedef struct {
    char* server;   // Server where the pong response must be sent
} PingEvent;

// Fired when a notice arrives
typedef struct {
    char* to;       // The destination of the message
    char* text;     // The text of the message
} NoticeEvent;

// Fired when a user joins a channel
typedef struct {
    UserInfo user;  // The user who joined a channel
    char* channel;  // The channel name
} JoinEvent;

// Fired when a user leaves a channel
typedef struct {
    UserInfo user;  // The user who leaved the channel
    char* channel;  // The channel name
    char* message;  // The part message
} PartEvent;

// Fired when a message is sent to a channel or to a user
typedef struct {
    UserInfo user;  // The user who sends the event
    char* to;       // The destination of the event (nick or channel)
    char* message;  // The text of the message
} MessageEvent;


/* ************************ */
/* Event building functions */ 
/* ************************ */

ErrorEvent      error_event(struct raw_msg *raw);
GenericEvent    generic_event(struct raw_msg *raw);
PingEvent       ping_event(struct raw_msg *raw);
NoticeEvent     notice_event(struct raw_msg *raw);
JoinEvent       join_event(struct raw_msg *raw);
PartEvent       part_event(struct raw_msg *raw);
MessageEvent    message_event(struct raw_msg *raw);

/* ************** */
/* Callback types */
/* ************** */

typedef void (*ErrorCallback)(ErrorEvent*);
typedef void (*GenericCallback)(GenericEvent*);
typedef void (*NoticeCallback)(NoticeEvent*);
typedef void (*JoinCallback)(JoinEvent*);
typedef void (*PartCallback)(PartEvent*);
typedef void (*MessageCallback)(MessageEvent*);

#endif

