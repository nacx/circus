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

#define _POSIX_C_SOURCE 200112L      /* Use snprintf */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "irc.h"

/* ************************ */
/* String utility functions */
/* ************************ */

void upper(char* str) {
    int i;
    if (str != NULL) {
        for (i = 0; i < strlen(str); i++) {
            str[i] = toupper(str[i]);
        }
    }
}

void lower(char* str) {
    int i;
    if (str != NULL) {
        for (i = 0; i < strlen(str); i++) {
            str[i] = tolower(str[i]);
        }
    }
}

/* ************************* */
/* Binding utility functions */
/* ************************* */

void build_command_key(char* key, char* command) {
    snprintf(key, 50, "%s#%s", PRIVMSG, command);
}

/* ********************* */
/* IRC utility functions */
/* ********************* */

void append_channel_flags(char* str, unsigned short int flags) {
    if (flags & CH_PRIVATE)     strcat(str, "p");
    if (flags & CH_SECRET)      strcat(str, "s");
    if (flags & CH_INVITEONLY)  strcat(str, "i");
    if (flags & CH_TOPICLOCK)   strcat(str, "t");
    if (flags & CH_NOEXTMSGS)   strcat(str, "n");
    if (flags & CH_MODERATED)   strcat(str, "m");

    /* This method is only intended to be used to parse flags that don't
     * require arguments.
     * CH_OPERATOR, CH_BAN, CH_LIMIT, CH_VOICE and CH_KEY flags must
     * be set using their individual functions. */
}

void append_user_flags(char* str, unsigned short int flags) {
    if (flags & USR_INVISIBLE)     strcat(str, "i");
    if (flags & USR_WALLOPS)       strcat(str, "w");
    if (flags & USR_OPERATOR)      strcat(str, "o");
}

