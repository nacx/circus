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
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "minunit.h"
#include "test.h"
#include "../lib/binding.h"
#include "../lib/irc.c"


/* The mock socket where the messages will be read */
FILE* mock_socket;

/* Read the data from the mock socket */
void read_mock(char* msg) {
    char* ret = fgets(msg, READ_BUF, mock_socket);
    mu_assert(ret != NULL, "read_mock: fgets should not return NULL");
}

void test_shutdown_handler() {
    shutdown_handler(SIGALRM);
    mu_assert(shutdown_requested == 0, "test_shutdown_handler: shutdown_requested should be 0 after unhandled signals");

    shutdown_handler(SIGHUP);
    mu_assert(shutdown_requested == 1, "test_shutdown_handler: shutdown_requested should be 1 after SIGHUP");
    shutdown_requested = 0;

    shutdown_handler(SIGTERM);
    mu_assert(shutdown_requested == 1, "test_shutdown_handler: shutdown_requested should be 1 after SIGTERM");
    shutdown_requested = 0;

    shutdown_handler(SIGINT);
    mu_assert(shutdown_requested == 1, "test_shutdown_handler: shutdown_requested should be 1 after SIGINT");
    shutdown_requested = 0;
}

void test_bind_event() {
    CallbackPtr binding = NULL;

    irc_bind_event("foo", (CallbackPtr) test_bind_event);
    binding = lookup_event("foo");
    mu_assert(binding == (CallbackPtr) test_bind_event, "test_bind_event: bound function is not the same");
}

void test_unbind_event() {
    irc_bind_event("foo", (CallbackPtr) test_unbind_event);
    irc_unbind_event("foo");

    mu_assert(lookup_event("foo") == NULL, "test_unbind_event: binding should be null");
}

void test_bind_command() {
    CallbackPtr binding = NULL;

    irc_bind_command("foo", (CallbackPtr) test_bind_command);
    binding = lookup_event("PRIVMSG#foo");
    mu_assert(binding == (CallbackPtr) test_bind_command, "test_bind_command: bound function is not the same");
}

void test_unbind_command() {
    irc_bind_command("foo", (CallbackPtr) test_bind_event);
    irc_unbind_command("foo");

    mu_assert(lookup_event("PRIVMSG#foo") == NULL, "test_unbind_command: binding should be null");
}

void test_irc_nick() {
    char msg[READ_BUF];

    irc_nick("circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "NICK circus\r\n"), "test_irc_nick: msg should be 'NICK circus\\r\\n'");
}

void test_irc_user() {
    char msg[READ_BUF];

    irc_user("Circus", "Circus IRC");
    read_mock(msg);

    mu_assert(s_eq(msg, "USER Circus hostname server :Circus IRC\r\n"),
            "test_irc_user: msg should be 'USER Circus hostname server :Circus IRC\\r\\n'");
}

void test_irc_quit() {
    char msg[READ_BUF];

    irc_quit("Bye");
    read_mock(msg);

    mu_assert(s_eq(msg, "QUIT :Bye\r\n"), "test_irc_quit: msg should be 'QUIT :Bye\\r\\n'");
}

void test_irc_join() {
    char msg[READ_BUF];

    irc_join("#circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "JOIN #circus\r\n"), "test_irc_join: msg should be 'JOIN #circus\\r\\n'");
}

void test_irc_join_pass() {
    char msg[READ_BUF];

    irc_join_pass("#circus", "foo");
    read_mock(msg);

    mu_assert(s_eq(msg, "JOIN #circus foo\r\n"), "test_irc_join_pass: msg should be 'JOIN #circus foo\\r\\n'");
}

void test_irc_part() {
    char msg[READ_BUF];

    irc_part("#circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "PART #circus\r\n"), "test_irc_part: msg should be 'PART #circus\\r\\n'");
}

void test_irc_topic() {
    char msg[READ_BUF];

    irc_topic("#circus", "The topic");
    read_mock(msg);

    mu_assert(s_eq(msg, "TOPIC #circus :The topic\r\n"), "test_irc_topic: msg should be 'TOPIC #circus :The topic\\r\\n'");
}

void test_irc_names() {
    char msg[READ_BUF];

    irc_names("#circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "NAMES #circus\r\n"), "test_irc_names: msg should be 'NAMES #circus\\r\\n'");
}

void test_irc_list() {
    char msg[READ_BUF];

    irc_list();
    read_mock(msg);

    mu_assert(s_eq(msg, "LIST\r\n"), "test_irc_list: msg should be 'LIST\\r\\n'");
}

void test_irc_invite() {
    char msg[READ_BUF];

    irc_invite("circus", "#circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "INVITE circus #circus\r\n"), "test_irc_invite: msg should be 'INVITE circus #circus\\r\\n'");
}

void test_irc_channel_msg() {
    char msg[READ_BUF];

    irc_channel_msg("#circus", "Foo bar");
    read_mock(msg);

    mu_assert(s_eq(msg, "PRIVMSG #circus :Foo bar\r\n"),
            "test_irc_channel_msg: msg should be 'PRIVMSG #circus :Foo bar\\r\\n'");
}

void test_irc_private_msg() {
    char msg[READ_BUF];

    irc_private_msg("circus", "Foo bar");
    read_mock(msg);

    mu_assert(s_eq(msg, "PRIVMSG circus :Foo bar\r\n"),
            "test_irc_private_msg: msg should be 'PRIVMSG circus :Foo bar\\r\\n'");
}

void test_irc_op() {
    char msg[READ_BUF];

    irc_op("#circus", "circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +o circus\r\n"), "test_irc_op: msg should be 'MODE #circus +o circus\\r\\n'");
}

void test_irc_deop() {
    char msg[READ_BUF];

    irc_deop("#circus", "circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus -o circus\r\n"), "test_irc_deop: msg should be 'MODE #circus -o circus\\r\\n'");
}

void test_irc_voice() {
    char msg[READ_BUF];

    irc_voice("#circus", "circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +v circus\r\n"), "test_irc_voice: msg should be 'MODE #circus +v circus\\r\\n'");
}

void test_irc_devoice() {
    char msg[READ_BUF];

    irc_devoice("#circus", "circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus -v circus\r\n"), "test_irc_devoice: msg should be 'MODE #circus -v circus\\r\\n'");
}

void test_irc_kick() {
    char msg[READ_BUF];

    irc_kick("#circus", "circus", "Foo");
    read_mock(msg);

    mu_assert(s_eq(msg, "KICK #circus circus :Foo\r\n"), "test_irc_kick: msg should be 'KICK #circus circus :Foo\\r\\n'");
}

void test_irc_ban() {
    char msg[READ_BUF];

    irc_ban("#circus", "*!*@*");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +b *!*@*\r\n"), "test_irc_ban: msg should be 'MODE #circus +b *!*@*\\r\\n'");
}

void test_irc_unban() {
    char msg[READ_BUF];

    irc_unban("#circus", "*!*@*");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus -b *!*@*\r\n"), "test_irc_unban: msg should be 'MODE #circus -b *!*@*\\r\\n'");
}

void test_irc_ban_list() {
    char msg[READ_BUF];

    irc_ban_list("#circus");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +b\r\n"), "test_irc_ban_list: msg should be 'MODE #circus +b\\r\\n'");
}

void test_irc_limit() {
    char msg[READ_BUF];

    irc_limit("#circus", 10);
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +l 10\r\n"), "test_irc_limit: msg should be 'MODE #circus +l 10\\r\\n'");
}

void test_irc_channel_key() {
    char msg[READ_BUF];

    irc_channel_key("#circus", "Foo");
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +k Foo\r\n"), "test_irc_channel_key: msg should be 'MODE #circus +k Foo\\r\\n'");
}

void test_irc_channel_set() {
    char msg[READ_BUF];

    irc_channel_set("#circus", CH_PRIVATE | CH_SECRET);
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus +ps\r\n"), "test_irc_channel_set: msg should be 'MODE #circus +ps\\r\\n'");
}

void test_irc_channel_unset() {
    char msg[READ_BUF];

    irc_channel_unset("#circus", CH_PRIVATE | CH_SECRET);
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE #circus -ps\r\n"), "test_irc_channel_unset: msg should be 'MODE #circus -ps\\r\\n'");
}

void test_irc_user_set() {
    char msg[READ_BUF];

    irc_user_set("circus", USR_INVISIBLE | USR_OPERATOR);
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE circus +io\r\n"), "test_irc_user_set: msg should be 'MODE circus +io\\r\\n'");
}

void test_irc_user_unset() {
    char msg[READ_BUF];

    irc_user_unset("circus", USR_INVISIBLE | USR_OPERATOR);
    read_mock(msg);

    mu_assert(s_eq(msg, "MODE circus -io\r\n"), "test_irc_user_unset: msg should be 'MODE circus -io\\r\\n'");
}

void test_irc_pong() {
    char msg[READ_BUF];

    irc_pong("irc.freenode.net");
    read_mock(msg);

    mu_assert(s_eq(msg, "PONG irc.freenode.net\r\n"), "test_irc_pong: msg should be 'PONG irc.freenode.net\\r\\n'");
}

void test_irc_raw() {
    char msg[READ_BUF];

    irc_raw("prefix", "type", "message");
    read_mock(msg);

    mu_assert(s_eq(msg, ":prefix type message\r\n"), "test_irc_raw: msg should be ':prefix type message\\r\\n'");
}

void test_irc() {
    int socks[2];

    mu_run(test_shutdown_handler);
    mu_run(test_bind_event);
    mu_run(test_unbind_event);
    mu_run(test_bind_command);
    mu_run(test_unbind_command);

    /* Mock the socket used to test the IRC messages */
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1) {
        perror("socketpair error");
        exit(EXIT_FAILURE);
    }

    _socket = socks[0];
    mock_socket = fdopen(socks[1], "r");

    mu_run(test_irc_nick);
    mu_run(test_irc_user);
    mu_run(test_irc_quit);
    mu_run(test_irc_join);
    mu_run(test_irc_join_pass);
    mu_run(test_irc_part);
    mu_run(test_irc_topic);
    mu_run(test_irc_names);
    mu_run(test_irc_list);
    mu_run(test_irc_invite);
    mu_run(test_irc_channel_msg);
    mu_run(test_irc_private_msg);
    mu_run(test_irc_op);
    mu_run(test_irc_deop);
    mu_run(test_irc_voice);
    mu_run(test_irc_devoice);
    mu_run(test_irc_kick);
    mu_run(test_irc_ban);
    mu_run(test_irc_unban);
    mu_run(test_irc_ban_list);
    mu_run(test_irc_limit);
    mu_run(test_irc_channel_key);
    mu_run(test_irc_channel_set);
    mu_run(test_irc_channel_unset);
    mu_run(test_irc_user_set);
    mu_run(test_irc_user_unset);
    mu_run(test_irc_pong);
    mu_run(test_irc_raw);

    close(socks[0]);
    close(socks[1]);
}

