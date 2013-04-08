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
#include "minunit.h"
#include "test.h"
#include "../lib/utils.h"
#include "../lib/irc.h"
#include "../lib/binding.h"
#include "../lib/message_handler.c"


/* Event counters */
int nick_events = 0;
int quit_events = 0;
int join_events = 0;
int part_events = 0;
int topic_events = 0;
int names_events = 0;
int list_events = 0;
int invite_events = 0;
int kick_events = 0;
int message_events = 0;
int mode_events = 0;
int ping_events = 0;
int notice_events = 0;
int error_events = 0;
int generic_events = 0;

/* Handler functions */
void on_nick(NickEvent* event) { nick_events++; }
void on_quit(QuitEvent* event) { quit_events++; }
void on_join(JoinEvent* event) { join_events++; }
void on_part(PartEvent* event) { part_events++; }
void on_topic(TopicEvent* event) { topic_events++; }
void on_names(NamesEvent* event) { names_events++; }
void on_list(ListEvent* event) { list_events++; }
void on_invite(InviteEvent* event) { invite_events++; }
void on_kick(KickEvent* event) { kick_events++; }
void on_message(MessageEvent* event) { message_events++; }
void on_mode(ModeEvent* event) { mode_events++; }
void on_ping(PingEvent* event) { ping_events++; }
void on_notice(NoticeEvent* event) { notice_events++; }
void on_error(ErrorEvent* event) { error_events++; }
void on_generic(GenericEvent* event) { generic_events++; }


void test_new_raw_message() {
    int i;
    struct raw_msg raw = new_raw_message();

    mu_assert(raw.timestamp.tv_sec > 0, "test_new_raw_message: timestamp seconds should be > 0");
    mu_assert(raw.timestamp.tv_usec > 0, "test_new_raw_message: timestamp microseconds should be > 0");
    mu_assert(raw.prefix == NULL, "test_new_raw_message: prefix should be NULL");
    mu_assert(raw.type == NULL, "test_new_raw_message: type should be NULL");
    mu_assert(raw.num_params == 0, "test_new_raw_message: num_params should be '0'");
    for (i = 0; i < MAX_PARAMS; i++) {
        mu_assert(raw.params[i] == NULL, "test_new_raw_message: params[i] should be NULL");
    }
}

void test_parse_empty_message() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(NULL, buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    free(buffer);   /* Cleanup */


    raw = parse("", buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse("TEST This is a message test without prefix", buffer);

    mu_assert(raw.prefix == NULL, "test_parse: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse: type should be TEST"); 
    mu_assert(raw.num_params == 7, "test_parse: there should be 7 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":prefix TEST This is a message test with prefix", buffer);

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix: type should be 'TEST'"); 
    mu_assert(raw.num_params == 7, "test_parse_with_prefix: there should be 7 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse_with_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse("TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params -1];

    mu_assert(raw.prefix == NULL, "test_parse_with_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_last_param: last parameter should be 'last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix_and_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":prefix TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params - 1];

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix_and_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix_and_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_prefix_and_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_prefix_and_last_param: last parameter should be 'last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_only_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse("TEST :only last parameter", buffer);
    last_param = raw.params[raw.num_params -1];

    mu_assert(raw.prefix == NULL, "test_parse_only_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_only_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 1, "test_parse_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_only_last_param: last parameter should be 'only last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix_only_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":prefix TEST :only last parameter", buffer);
    last_param = raw.params[raw.num_params - 1];

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix_only_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix_only_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 1, "test_parse_with_prefix_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_with_prefix_only_last_param: last parameter should be 'only last parameter'");

    free(buffer);   /* Cleanup */
}

void test_fire_nick_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(NICK, (CallbackPtr) on_nick);
    raw = parse("NICK test-nick", buffer);
    fire_event(&raw);

    mu_assert(nick_events == 1, "test_fire_nick_event: nick_events should be '1'");

    irc_unbind_event(NICK);
    free(buffer);
}

void test_fire_quit_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(QUIT, (CallbackPtr) on_quit);
    raw = parse("QUIT :Bye bye!", buffer);
    fire_event(&raw);

    mu_assert(quit_events == 1, "test_fire_quit_event: quit_events should be '1'");

    irc_unbind_event(QUIT);
    free(buffer);
}

void test_fire_join_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(JOIN, (CallbackPtr) on_join);
    raw = parse("JOIN #circus", buffer);
    fire_event(&raw);

    mu_assert(join_events == 1, "test_fire_join_event: join_events should be '1'");

    irc_unbind_event(JOIN);
    free(buffer);
}

void test_fire_part_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(PART, (CallbackPtr) on_part);
    raw = parse("PART #circus :Bye", buffer);
    fire_event(&raw);

    mu_assert(part_events == 1, "test_fire_part_event: part_events should be '1'");

    irc_unbind_event(PART);
    free(buffer);
}

void test_fire_topic_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(TOPIC, (CallbackPtr) on_topic);
    raw = parse("TOPIC #circus :New topic", buffer);
    fire_event(&raw);

    mu_assert(topic_events == 1, "test_fire_topic_event: topic_events should be '1'");

    irc_unbind_event(TOPIC);
    free(buffer);
}

void test_fire_names_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(NAMES, (CallbackPtr) on_names);

    raw = parse("353 test-nick @ #circus :test1 test2", buffer);
    fire_event(&raw);
    mu_assert(names_events == 1, "test_fire_names_event: names_events should be '1'");
    free(buffer);

    raw = parse("366 test-nick #circus :End of /NAMES list", buffer);
    fire_event(&raw);
    mu_assert(names_events == 2, "test_fire_names_event: names_events should be '2'");
    free(buffer);

    irc_unbind_event(NAMES);
}

void test_fire_list_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(LIST, (CallbackPtr) on_list);

    raw = parse(":moorcock.freenode.net 322 circus-bot #circus 7 :Circus IRC framework", buffer);
    fire_event(&raw);
    mu_assert(list_events == 1, "test_fire_list_event: list_events should be '1'");
    free(buffer);

    raw = parse(":moorcock.freenode.net 323 circus-bot :End of /LIST", buffer);
    fire_event(&raw);
    mu_assert(list_events == 2, "test_fire_list_event: list_events should be '2'");
    free(buffer);

    irc_unbind_event(LIST);
}

void test_fire_invite_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(INVITE, (CallbackPtr) on_invite);
    raw = parse(":nacx!~nacx@127.0.0.1 INVITE circus-bot :#circus", buffer);
    fire_event(&raw);

    mu_assert(invite_events == 1, "test_fire_invite_event: invite_events should be '1'");

    irc_unbind_event(INVITE);
    free(buffer);
}

void test_fire_kick_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(KICK, (CallbackPtr) on_kick);
    raw = parse(":nacx!~nacx@127.0.0.1 KICK #circus circus-bot :Foo", buffer);
    fire_event(&raw);

    mu_assert(kick_events == 1, "test_fire_kick_event: kick_events should be '1'");

    irc_unbind_event(KICK);
    free(buffer);
}

void test_fire_message_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(PRIVMSG, (CallbackPtr) on_message);

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :Hi there", buffer);
    fire_event(&raw);
    mu_assert(message_events == 1, "test_fire_message_event: message_events should be '1'");
    free(buffer);

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :Hi there", buffer);
    fire_event(&raw);
    mu_assert(message_events == 2, "test_fire_message_event: message_events should be '2'");
    free(buffer);

    irc_unbind_event(PRIVMSG);
    irc_bind_command("!cmd", (CallbackPtr) on_message);

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :!cmd Do it", buffer);
    fire_event(&raw);
    mu_assert(message_events == 3, "test_fire_message_event: message_events should be '3'");
    free(buffer);

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :!cmd", buffer);
    fire_event(&raw);
    mu_assert(message_events == 4, "test_fire_message_event: message_events should be '2'");
    free(buffer);

    irc_unbind_command("!cmd");
}

void test_fire_mode_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(MODE, (CallbackPtr) on_mode);
    raw = parse(":nick!~user@server MODE #test +inm", buffer);
    fire_event(&raw);

    mu_assert(mode_events == 1, "test_fire_mode_event: mode_events should be '1'");

    irc_unbind_event(MODE);
    free(buffer);
}

void test_fire_ping_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(PING, (CallbackPtr) on_ping);
    raw = parse("PING :zelazny.freenode.net", buffer);
    fire_event(&raw);

    mu_assert(ping_events == 1, "test_fire_ping_event: ping_events should be '1'");

    irc_unbind_event(PING);
    free(buffer);
}

void test_fire_notice_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    irc_bind_event(NOTICE, (CallbackPtr) on_notice);
    raw = parse(":moorcock.freenode.net NOTICE * :Message", buffer);
    fire_event(&raw);

    mu_assert(notice_events == 1, "test_fire_notice_event: notice_events should be '1'");

    irc_unbind_event(NOTICE);
    free(buffer);
}

void test_fire_error_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":nick!~user@server 401 circus-bot :Test message", buffer);

    irc_bind_event(ERR_NOSUCHNICK, (CallbackPtr) on_error);
    fire_event(&raw);
    mu_assert(error_events == 1, "test_fire_error_event: error_events should be '1'");

    irc_unbind_event(ERR_NOSUCHNICK);
    irc_bind_event(ERROR, (CallbackPtr) on_error);
    fire_event(&raw);
    mu_assert(error_events == 2, "test_fire_error_event: error_events should be '2'");

    free(buffer);
}

void test_fire_generic_event() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":nick!~user@server 305 circus-bot :Test message", buffer);

    irc_bind_event(RPL_UNAWAY, (CallbackPtr) on_generic);
    fire_event(&raw);
    mu_assert(generic_events == 1, "test_fire_generic_event: generic_events should be '1'");

    irc_unbind_event(RPL_UNAWAY);
    irc_bind_event(ALL, (CallbackPtr) on_generic);
    fire_event(&raw);
    mu_assert(generic_events == 2, "test_fire_generic_event: generic_events should be '2'");

    free(buffer);
}

void test_message_handler() {
    mu_run(test_new_raw_message);
    mu_run(test_parse_empty_message);
    mu_run(test_parse);
    mu_run(test_parse_with_prefix);
    mu_run(test_parse_with_last_param);
    mu_run(test_parse_with_prefix_and_last_param);
    mu_run(test_parse_only_last_param);
    mu_run(test_parse_with_prefix_only_last_param);

    mu_run(test_fire_nick_event);
    mu_run(test_fire_quit_event);
    mu_run(test_fire_join_event);
    mu_run(test_fire_part_event);
    mu_run(test_fire_topic_event);
    mu_run(test_fire_names_event);
    mu_run(test_fire_list_event);
    mu_run(test_fire_invite_event);
    mu_run(test_fire_kick_event);
    mu_run(test_fire_message_event);
    mu_run(test_fire_mode_event);
    mu_run(test_fire_ping_event);
    mu_run(test_fire_notice_event);
    mu_run(test_fire_error_event);
    mu_run(test_fire_generic_event);

    cleanup_bindings();
}

