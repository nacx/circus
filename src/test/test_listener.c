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
#include "../lib/listener.c"


/* Event counters */
int evt_nicks = 0;
int evt_quits = 0;
int evt_joins = 0;
int evt_parts = 0;
int evt_topics = 0;
int evt_namess = 0;
int evt_lists = 0;
int evt_invites = 0;
int evt_kicks = 0;
int evt_messages = 0;
int evt_modes = 0;
int evt_pings = 0;
int evt_notices = 0;
int evt_errors = 0;
int evt_generics = 0;

/* Handler functions */
void on_nick(NickEvent* event) { evt_nicks++; }
void on_quit(QuitEvent* event) { evt_quits++; }
void on_join(JoinEvent* event) { evt_joins++; }
void on_part(PartEvent* event) { evt_parts++; }
void on_topic(TopicEvent* event) { evt_topics++; }
void on_names(NamesEvent* event) { evt_namess++; }
void on_list(ListEvent* event) { evt_lists++; }
void on_invite(InviteEvent* event) { evt_invites++; }
void on_kick(KickEvent* event) { evt_kicks++; }
void on_message(MessageEvent* event) { evt_messages++; }
void on_mode(ModeEvent* event) { evt_modes++; }
void on_ping(PingEvent* event) { evt_pings++; }
void on_notice(NoticeEvent* event) { evt_notices++; }
void on_error(ErrorEvent* event) { evt_errors++; }
void on_generic(GenericEvent* event) { evt_generics++; }


void test_new_raw_message() {
    int i;
    struct raw_event raw = new_raw_message();

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
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(NULL, buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    free(buffer);   /* Cleanup */


    raw = lst_parse("", buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse() {
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse("TEST This is a message test without prefix", buffer);

    mu_assert(raw.prefix == NULL, "test_parse: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse: type should be TEST"); 
    mu_assert(raw.num_params == 7, "test_parse: there should be 7 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix() {
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(":prefix TEST This is a message test with prefix", buffer);

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix: type should be 'TEST'"); 
    mu_assert(raw.num_params == 7, "test_parse_with_prefix: there should be 7 parameters");

    free(buffer);   /* Cleanup */
}

void test_parse_with_last_param() {
    char* last_param;
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse("TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params -1];

    mu_assert(raw.prefix == NULL, "test_parse_with_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_last_param: last parameter should be 'last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix_and_last_param() {
    char* last_param;
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(":prefix TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params - 1];

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix_and_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix_and_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_prefix_and_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_prefix_and_last_param: last parameter should be 'last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_only_last_param() {
    char* last_param;
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse("TEST :only last parameter", buffer);
    last_param = raw.params[raw.num_params -1];

    mu_assert(raw.prefix == NULL, "test_parse_only_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_only_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 1, "test_parse_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_only_last_param: last parameter should be 'only last parameter'");

    free(buffer);   /* Cleanup */
}

void test_parse_with_prefix_only_last_param() {
    char* last_param;
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(":prefix TEST :only last parameter", buffer);
    last_param = raw.params[raw.num_params - 1];

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix_only_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix_only_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 1, "test_parse_with_prefix_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_with_prefix_only_last_param: last parameter should be 'only last parameter'");

    free(buffer);   /* Cleanup */
}

void test_fire_evt_nick() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(NICK, (CallbackPtr) on_nick);
    raw = lst_parse("NICK test-nick", buffer);
    _fire_event(&raw);

    mu_assert(evt_nicks == 1, "test_fire_evt_nick: evt_nicks should be '1'");

    irc_unbind_event(NICK);
    free(buffer);
}

void test_fire_evt_quit() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(QUIT, (CallbackPtr) on_quit);
    raw = lst_parse("QUIT :Bye bye!", buffer);
    _fire_event(&raw);

    mu_assert(evt_quits == 1, "test_fire_evt_quit: evt_quits should be '1'");

    irc_unbind_event(QUIT);
    free(buffer);
}

void test_fire_evt_join() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(JOIN, (CallbackPtr) on_join);
    raw = lst_parse("JOIN #circus", buffer);
    _fire_event(&raw);

    mu_assert(evt_joins == 1, "test_fire_evt_join: evt_joins should be '1'");

    irc_unbind_event(JOIN);
    free(buffer);
}

void test_fire_evt_part() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(PART, (CallbackPtr) on_part);
    raw = lst_parse("PART #circus :Bye", buffer);
    _fire_event(&raw);

    mu_assert(evt_parts == 1, "test_fire_evt_part: evt_parts should be '1'");

    irc_unbind_event(PART);
    free(buffer);
}

void test_fire_evt_topic() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(TOPIC, (CallbackPtr) on_topic);
    raw = lst_parse("TOPIC #circus :New topic", buffer);
    _fire_event(&raw);

    mu_assert(evt_topics == 1, "test_fire_evt_topic: evt_topics should be '1'");

    irc_unbind_event(TOPIC);
    free(buffer);
}

void test_fire_evt_names() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(NAMES, (CallbackPtr) on_names);

    raw = lst_parse("353 test-nick @ #circus :test1 test2", buffer);
    _fire_event(&raw);
    mu_assert(evt_namess == 1, "test_fire_evt_names: evt_namess should be '1'");
    free(buffer);

    raw = lst_parse("366 test-nick #circus :End of /NAMES list", buffer);
    _fire_event(&raw);
    mu_assert(evt_namess == 2, "test_fire_evt_names: evt_namess should be '2'");
    free(buffer);

    irc_unbind_event(NAMES);
}

void test_fire_evt_list() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(LIST, (CallbackPtr) on_list);

    raw = lst_parse(":moorcock.freenode.net 322 circus-bot #circus 7 :Circus IRC framework", buffer);
    _fire_event(&raw);
    mu_assert(evt_lists == 1, "test_fire_evt_list: evt_lists should be '1'");
    free(buffer);

    raw = lst_parse(":moorcock.freenode.net 323 circus-bot :End of /LIST", buffer);
    _fire_event(&raw);
    mu_assert(evt_lists == 2, "test_fire_evt_list: evt_lists should be '2'");
    free(buffer);

    irc_unbind_event(LIST);
}

void test_fire_evt_invite() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(INVITE, (CallbackPtr) on_invite);
    raw = lst_parse(":nacx!~nacx@127.0.0.1 INVITE circus-bot :#circus", buffer);
    _fire_event(&raw);

    mu_assert(evt_invites == 1, "test_fire_evt_invite: evt_invites should be '1'");

    irc_unbind_event(INVITE);
    free(buffer);
}

void test_fire_evt_kick() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(KICK, (CallbackPtr) on_kick);
    raw = lst_parse(":nacx!~nacx@127.0.0.1 KICK #circus circus-bot :Foo", buffer);
    _fire_event(&raw);

    mu_assert(evt_kicks == 1, "test_fire_evt_kick: evt_kicks should be '1'");

    irc_unbind_event(KICK);
    free(buffer);
}

void test_fire_evt_message() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(PRIVMSG, (CallbackPtr) on_message);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :Hi there", buffer);
    _fire_event(&raw);
    mu_assert(evt_messages == 1, "test_fire_evt_message: evt_messages should be '1'");
    free(buffer);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :Hi there", buffer);
    _fire_event(&raw);
    mu_assert(evt_messages == 2, "test_fire_evt_message: evt_messages should be '2'");
    free(buffer);

    irc_unbind_event(PRIVMSG);
    irc_bind_command("!cmd", (CallbackPtr) on_message);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :!cmd Do it", buffer);
    _fire_event(&raw);
    mu_assert(evt_messages == 3, "test_fire_evt_message: evt_messages should be '3'");
    free(buffer);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :!cmd", buffer);
    _fire_event(&raw);
    mu_assert(evt_messages == 4, "test_fire_evt_message: evt_messages should be '2'");
    free(buffer);

    irc_unbind_command("!cmd");
}

void test_fire_evt_mode() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(MODE, (CallbackPtr) on_mode);
    raw = lst_parse(":nick!~user@server MODE #test +inm", buffer);
    _fire_event(&raw);

    mu_assert(evt_modes == 1, "test_fire_evt_mode: evt_modes should be '1'");

    irc_unbind_event(MODE);
    free(buffer);
}

void test_fire_evt_ping() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(PING, (CallbackPtr) on_ping);
    raw = lst_parse("PING :zelazny.freenode.net", buffer);
    _fire_event(&raw);

    mu_assert(evt_pings == 1, "test_fire_evt_ping: evt_pings should be '1'");

    irc_unbind_event(PING);
    free(buffer);
}

void test_fire_evt_notice() {
    struct raw_event raw;
    char* buffer = NULL;

    irc_bind_event(NOTICE, (CallbackPtr) on_notice);
    raw = lst_parse(":moorcock.freenode.net NOTICE * :Message", buffer);
    _fire_event(&raw);

    mu_assert(evt_notices == 1, "test_fire_evt_notice: evt_notices should be '1'");

    irc_unbind_event(NOTICE);
    free(buffer);
}

void test_fire_evt_error() {
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(":nick!~user@server 401 circus-bot :Test message", buffer);

    irc_bind_event(ERR_NOSUCHNICK, (CallbackPtr) on_error);
    _fire_event(&raw);
    mu_assert(evt_errors == 1, "test_fire_evt_error: evt_errors should be '1'");

    irc_unbind_event(ERR_NOSUCHNICK);
    irc_bind_event(ERROR, (CallbackPtr) on_error);
    _fire_event(&raw);
    mu_assert(evt_errors == 2, "test_fire_evt_error: evt_errors should be '2'");

    free(buffer);
}

void test_fire_evt_generic() {
    struct raw_event raw;
    char* buffer = NULL;

    raw = lst_parse(":nick!~user@server 305 circus-bot :Test message", buffer);

    irc_bind_event(RPL_UNAWAY, (CallbackPtr) on_generic);
    _fire_event(&raw);
    mu_assert(evt_generics == 1, "test_fire_evt_generic: evt_generics should be '1'");

    irc_unbind_event(RPL_UNAWAY);
    irc_bind_event(ALL, (CallbackPtr) on_generic);
    _fire_event(&raw);
    mu_assert(evt_generics == 2, "test_fire_evt_generic: evt_generics should be '2'");

    free(buffer);
}

void test_listener() {
    mu_run(test_new_raw_message);
    mu_run(test_parse_empty_message);
    mu_run(test_parse);
    mu_run(test_parse_with_prefix);
    mu_run(test_parse_with_last_param);
    mu_run(test_parse_with_prefix_and_last_param);
    mu_run(test_parse_only_last_param);
    mu_run(test_parse_with_prefix_only_last_param);

    mu_run(test_fire_evt_nick);
    mu_run(test_fire_evt_quit);
    mu_run(test_fire_evt_join);
    mu_run(test_fire_evt_part);
    mu_run(test_fire_evt_topic);
    mu_run(test_fire_evt_names);
    mu_run(test_fire_evt_list);
    mu_run(test_fire_evt_invite);
    mu_run(test_fire_evt_kick);
    mu_run(test_fire_evt_message);
    mu_run(test_fire_evt_mode);
    mu_run(test_fire_evt_ping);
    mu_run(test_fire_evt_notice);
    mu_run(test_fire_evt_error);
    mu_run(test_fire_evt_generic);

    bnd_cleanup();
}

