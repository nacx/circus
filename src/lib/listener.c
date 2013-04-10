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
#include "events.h"
#include "listener.h"
#include "dispatcher.h"


/* *************** */
/* Message parsing */
/* *************** */

struct raw_event* lst_parse(char* msg) {
    int i = 0, is_last_parameter = 0;
    size_t msg_len;
    struct raw_event* raw = evt_raw_create();
    char* token, *token_end = NULL;

    if (msg != NULL && (msg_len = strlen(msg)) > 0) {

        if ((raw->__buffer = malloc((msg_len + 1) * sizeof(char))) == 0) {
            perror("Out of memory (parse)");
            exit(EXIT_FAILURE);
        }

        memset(raw->__buffer, '\0', msg_len + 1);
        strncpy(raw->__buffer, msg, msg_len);

        token = strtok_r(raw->__buffer, PARAM_SEP, &token_end);
        while(token != NULL)
        {
            if (raw->type == NULL) {
                /* If type is not set, we must check if there
                 * is a message prefix. */
                if (token[0] == ':') {
                    raw->prefix = token + 1; /* Ignore the ':' */
                } else {
                    raw->type = token;
                }
            } else { /* If the type is set the token is a parameter */
                /* If a parameter begins with ':' then it is
                 * the last parameter and it is all the remaining message. */
                if (token[0] == ':') {
                    /* Do not increment the parameter count */
                    raw->params[i] = token + 1; /* Ignore the ':' */
                    is_last_parameter = 1;       /* Set the last parameter flag */
                } else {
                    if (is_last_parameter == 0) {
                        raw->params[i++] = token;
                    } else {
                        /* If it is the last parameter, just concatenate
                         * the tokens. */
                        sprintf(raw->params[i], "%s %s", raw->params[i], token);
                    }
                }
            }

            /* Continue with the next token */
            token = strtok_r(NULL, PARAM_SEP, &token_end);
        }

        /* If we have a last parameter we should increment now the parameter
         * counter. */
        raw->num_params = (is_last_parameter == 0)? i : i + 1;
    }

    return raw;
}

void lst_handle(char* msg) {
    struct raw_event* raw;

    msg[strlen(msg) - 2] = '\0';    /* Remove the line terminaion before parsing */
    raw = lst_parse(msg);           /* Parse the input and get the raw event */

    dsp_dispatch(raw);      /* Send the event to the dispatcher thread */
}

