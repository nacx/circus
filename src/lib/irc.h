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

#ifndef __IRC_H__
#define __IRC_H__

// IRC message types
#define PING            "PING"
#define PONG            "PONG"
#define NOTICE          "NOTICE"
#define USER            "USER"
#define NICK            "NICK"
#define JOIN            "JOIN"
#define PART            "PART"
#define QUIT            "QUIT"
#define PRIVMSG         "PRIVMSG"
#define MODE            "MODE"
#define NICK_IN_USE     "433"

// Connection functions
void irc_connect(char* address, int port);
void irc_disconnect();
void irc_listen();

// Event binding functions
void irc_bind_event(char* event, void* callback);
void irc_bind_command(char* command, void* callback);
void irc_unbind_event(char* event);
void irc_unbind_command(char* command);

// System functions
void irc_pong(char* server);

// User functions
void irc_nick(char* nick);
void irc_user(char* user_name, char* real_name);
void irc_login(char* nick, char* user_name, char* real_name);

void irc_join(char* channel);
void irc_join_pass(char* channel, char* pass); 
void irc_part(char* channel); 
void irc_quit(char* message);

void irc_channel(char* channel, char* messge);
void irc_private(char* nick, char* message);

void irc_op(char* channel, char* nick);
void irc_deop(char* channel, char* nick);
void irc_voice(char* channel, char* nick);
void irc_devoice(char* channel, char* nick);

#endif

