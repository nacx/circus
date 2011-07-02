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

/* *************** */
/* IRC Event types */
/* *************** */

typedef struct {
    char* server;
} PingEvent;

typedef struct {
    char* nick;
    char* text;
} NoticeEvent;

typedef struct {
    char* nick;
    char* user;
    char* server;
    char* channel;
} JoinEvent;

typedef struct {
    char* nick;
    char* user;
    char* server;
    char* channel;
    char* message;
} PartEvent;

/* ************************ */
/* Event building functions */
/* ************************ */

PingEvent   ping_event(struct raw_msg * raw);
NoticeEvent notice_event(struct raw_msg * raw);
JoinEvent   join_event(struct raw_msg * raw);
PartEvent   part_event(struct raw_msg * raw);

/* ************** */
/* Callback types */
/* ************** */

typedef void (*NoticeCallback)(NoticeEvent*);
typedef void (*JoinCallback)(JoinEvent*);
typedef void (*PartCallback)(PartEvent*);

/* ********************************** */
/* User information utility functions */
/* ********************************** */

struct user_info {
    char* nick;
    char* user;
    char* server;
};

struct user_info get_user_info(char* user_ref);

#endif

