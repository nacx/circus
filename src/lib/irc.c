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
#include "network.h"
#include "irc.h"

// Connection functions

void irc_connect(char* address, int port) {
	net_connect(address, port);
}

void irc_disconnect(char* address, int port) {
	net_disconnect();
}

// User functions

void irc_nick(char* nick) {
	char msg[MSG_SIZE];
	snprintf(msg, MSG_SIZE, "NICK %s", nick);
	net_send(msg);
}

void irc_user(char* user_name, char* real_name) {
	char msg[MSG_SIZE];
	snprintf(msg, MSG_SIZE, "USER %s hostname server :%s", user_name, real_name);
	net_send(msg);
}

void irc_login(char* nick, char* user_name, char* real_name) {
	irc_nick(nick);
	irc_user(user_name, real_name);
}

void irc_quit(char* quit_msg) {
	char msg[MSG_SIZE];
	snprintf(msg, MSG_SIZE, "QUIT :%s", quit_msg);
	net_send(msg);
}
