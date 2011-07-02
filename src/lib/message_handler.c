/*
 * Copyright (c) 2010 Ignasi Barrera
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
#include <string.h>
#include "events.h"
#include "utils.h"
#include "binding.h"
#include "irc.h"
#include "message_handler.h"


void handle(char* msg) {
    struct raw_msg raw;
    char *line, *line_end, *buffer = NULL;

    if (msg != NULL && strlen(msg) > 0) {
        // Each line is a different IRC message
        line = strtok_r(msg, MSG_END, &line_end);
        while (line != NULL) {
            // Parse each message individually
            raw = parse(line, buffer);

            // Fire the event
            if (raw.type != NULL) {
                fire_event(&raw);
            }

            // Free memory used to parse the message
            // once it has been handled
            if (buffer != NULL) {
                free(buffer);
                buffer = NULL;
            }

            // Continue with the next line
            line = strtok_r(NULL, MSG_END, &line_end);
        }
    }
}

struct raw_msg new_raw_message() {
    int i;
    struct raw_msg raw;

    raw.prefix = NULL;
    raw.type = NULL;
    raw.num_params = 0;

    for (i = 0; i < MAX_PARAMS; i++) {
        raw.params[i] = NULL;
    }

    return raw;
}

struct raw_msg parse(char* msg, char* buffer) {
    int msg_len, i = 0, is_last_parameter = 0;
    struct raw_msg raw = new_raw_message();
    char *token, *token_end = NULL;

    if (msg != NULL && (msg_len = strlen(msg)) > 0) {

        if ((buffer = malloc(msg_len * sizeof(char*))) == 0) {
            perror("Out of memory (parse)");
            exit(EXIT_FAILURE);
        }

        strncpy(buffer, msg, msg_len);

        token = strtok_r(buffer, PARAM_SEP, &token_end);
        while(token != NULL)
        {
            if (raw.type == NULL) {
                // If type is not set, we must check if there
                // is a message prefix
                if (token[0] == ':') {
                    raw.prefix = token + 1; // Ignore the ':'
                } else {
                    raw.type = token;
                }
            } else { // If the type is set the token is a parameter
                // If a parameter begins with ':' then it is
                // the last parameter and it is all the remaining message
                if (token[0] == ':') {
                    // Do not increment the parameter count
                    raw.params[i] = token + 1; // Ignore the ':'
                    is_last_parameter = 1;       // Set the last parameter flag
                } else {
                    if (is_last_parameter == 0) {
                        raw.params[i++] = token;
                    } else {
                        // If it is the last parameter, just concatenate
                        // the tokens
                        sprintf(raw.params[i], "%s %s", raw.params[i], token);
                    }
                }
            }

            // Continue with the next token
            token = strtok_r(NULL, PARAM_SEP, &token_end);
        }

        // If we have a last parameter we should increment now the parameter
        // counter
        raw.num_params = (is_last_parameter == 0)? i : i + 1;
    }

    return raw;
}

void fire_event(struct raw_msg *raw) {
    upper(raw->type);

    if (s_eq(raw->type, PING)) {
        PingEvent event = ping_event(raw);
        on_ping(&event);
    } else if (s_eq(raw->type, NOTICE)) {
        NoticeEvent event = notice_event(raw);
        void* callback = lookup_event(raw->type);
        if (callback != NULL) {
            ((NoticeCallback) callback)(&event);
        }
    } else if (s_eq(raw->type, JOIN)) {
        JoinEvent event = join_event(raw);
        void* callback = lookup_event(raw->type);
        if (callback != NULL) {
            ((JoinCallback) callback)(&event);
        }
    } else if (s_eq(raw->type, PART)) {
        PartEvent event = part_event(raw);
        void* callback = lookup_event(raw->type);
        if (callback != NULL) {
            ((PartCallback) callback)(&event);
        }
    }

    // Just ignore other message types
}

