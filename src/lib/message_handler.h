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

#ifndef __MESSAGEHANDLER_H__
#define __MESSAGEHANDLER_H__

#define s_eq(a,b)   strcmp(a,b) == 0

// Maximum number of parameters as defined int he prococol
#define MAX_PARAMS 15
#define MSG_END    "\r\n"
#define PARAM_SEP  " "

// Raw IRC message
struct raw_msg{
    char* type;                 // The IRC message type
    char* prefix;               // The message prefix (if any)
    int   num_params;           // The number of parameters
    char* params[MAX_PARAMS];   // The parameter array
};

// Message parsing functions
void handle(char* msg);
struct raw_msg parse(char* msg, char* buffer);
void fire_event(struct raw_msg *raw);

#endif

