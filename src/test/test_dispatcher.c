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

#include <poll.h>
#include "minunit.h"
#include "test.h"
#include "../lib/events.h"
#include "../lib/binding.h"
#include "../lib/irc.h"
#include "../lib/listener.h"
#include "../lib/dispatcher.c"


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
int evt_dispatch = 0;

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
void on_dispatch(GenericEvent* event) { evt_dispatch++; }


void test_queue_create_destroy() {
    q_create();
    mu_assert(events != NULL, "test_queue_create_destroy: events should not be NULL");
    mu_assert(events->lock != NULL, "test_queue_create_destroy: events->lock should not be NULL");
    mu_assert(events->top == NULL, "test_queue_create_destroy: events->top shouldbe NULL");
    mu_assert(events->bottom == NULL, "test_queue_create_destroy: events->bottom should be NULL");
    mu_assert(events->size == 0, "test_queue_create_destroy: events size should be 0");

    q_destroy();
    mu_assert(events == NULL, "test_queue_create_destroy: events should be NULL");
}

void test_queue_push_single() {
    struct raw_event* raw;

    raw = lst_parse(":prefix TEST This is a message test with a :last parameter");
    q_create();
    q_push(raw);

    mu_assert(events->size == 1, "test_queue_push_single: events size should be 1");
    mu_assert(events->top != NULL, "test_queue_push_single: events->top should not be NULL");
    mu_assert(events->bottom != NULL, "test_queue_push_single: events->bottom should not be NULL");
    mu_assert(events->top == events->bottom,
            "test_queue_push_single: events->top and events->bottom should be the same");
    mu_assert(events->top->event == raw, "test_queue_push_single: events->top->event should be the raw event");
    mu_assert(events->top->prev == NULL, "test_queue_push_single: events->top->prev should be NULL");
    mu_assert(events->top->next == NULL, "test_queue_push_single: events->top->next should be NULL");

    q_destroy();
    evt_raw_destroy(raw);
}

void test_queue_push_many() {
    struct raw_event* raw1, *raw2;

    raw1 = lst_parse(":prefix TEST1 This is a message test with a :last parameter");
    raw2 = lst_parse(":prefix TEST2 This is a message test with a :last parameter");
    q_create();
    q_push(raw1);
    q_push(raw2);

    mu_assert(events->size == 2, "test_queue_push_many: events size should be 2");
    mu_assert(events->top != NULL, "test_queue_push_many: events->top should not be NULL");
    mu_assert(events->bottom != NULL, "test_queue_push_many: events->bottom should not be NULL");
    mu_assert(events->top != events->bottom,
            "test_queue_push_many: events->top and events->bottom should not be the same");
    mu_assert(events->top->next != NULL, "test_queue_push_many: events->top->next should not be NULL");
    mu_assert(events->top->prev == NULL, "test_queue_push_many: events->top->prev should be NULL");
    mu_assert(events->top->next->prev == events->top,
            "test_queue_push_many: events->top->next->prev should not be the top");
    mu_assert(events->top->next->next == NULL, "test_queue_push_many: events->top->next->next should be NULL");
    mu_assert(events->top->event == raw1, "test_queue_push_many: events->top->event should be the raw1 event");
    mu_assert(events->bottom->event == raw2, "test_queue_push_many: events->bottom->event should be the raw2 event");
    mu_assert(events->top->next->event == raw2, "test_queue_push_many: events->top->next->event should be the raw2 event");

    q_destroy();
    evt_raw_destroy(raw1);
    evt_raw_destroy(raw2);
}

void test_queue_pop_single() {
    struct raw_event* raw, *result;

    raw = lst_parse(":prefix TEST This is a message test with a :last parameter");
    q_create();
    q_push(raw);
    result = q_pop();

    mu_assert(events->size == 0, "test_queue_pop_single: events size should be 0");
    mu_assert(events->top == NULL, "test_queue_pop_single: events->top should be NULL");
    mu_assert(events->bottom == NULL, "test_queue_pop_single: events->bottom should be NULL");
    mu_assert(result == raw, "test_queue_pop_single: the returned value should be the added event");

    q_destroy();
    evt_raw_destroy(raw);
}

void test_queue_pop_many() {
    struct raw_event* raw1, *raw2, *result;

    raw1 = lst_parse(":prefix TEST1 This is a message test with a :last parameter");
    raw2 = lst_parse(":prefix TEST2 This is a message test with a :last parameter");
    q_create();
    q_push(raw1);
    q_push(raw2);
    result = q_pop();

    mu_assert(result == raw1, "test_queue_pop_many: result should be raw1");
    mu_assert(events->size == 1, "test_queue_pop_many: events size should be 1");
    mu_assert(events->top != NULL, "test_queue_pop_many: events->top should not be NULL");
    mu_assert(events->bottom != NULL, "test_queue_pop_many: events->bottom should not be NULL");
    mu_assert(events->top == events->bottom,
            "test_queue_pop_many: events->top and events->bottom should be the same");
    mu_assert(events->top->next == NULL, "test_queue_pop_many: events->top->next should be NULL");
    mu_assert(events->top->prev == NULL, "test_queue_pop_many: events->top->prev should be NULL");
    mu_assert(events->top->event == raw2, "test_queue_pop_many: events->top->event should be the raw2 event");
    mu_assert(events->bottom->event == raw2, "test_queue_pop_many: events->bottom->event should be the raw2 event");

    q_destroy();
    evt_raw_destroy(raw1);
    evt_raw_destroy(raw2);
}

void test_consumer_create_destroy() {
    consumer_create();

    mu_assert(consumer != NULL, "test_consumer_create_destroy: consumer should not be NULL");
    mu_assert(consumer->worker != NULL, "test_consumer_create_destroy: consumer->worker should not be NULL");
    mu_assert(consumer->lock != NULL, "test_consumer_create_destroy: consumer->lock should not be NULL");
    mu_assert(consumer->ready != NULL, "test_consumer_create_destroy: consumer->ready should not be NULL");
    mu_assert(consumer->terminate == 0, "test_consumer_create_destroy: consumer->terminate should not be '0'");

    consumer_destroy();
    mu_assert(consumer == NULL, "test_consumer_create_destroy: consumer should be NULL");
}

void test_dsp_start_shutdown() {
    dsp_start();
    mu_assert(events != NULL, "test_dsp_start_shutdown: events should not be NULL");
    mu_assert(consumer != NULL, "test_dsp_start_shutdown: consumer should not be NULL");

    dsp_shutdown();
    mu_assert(consumer == NULL, "test_dsp_start_shutdown: consumer should be NULL");
    mu_assert(events == NULL, "test_dsp_start_shutdown: events should be NULL");
}

void test_dsp_dispatch() {
    struct raw_event* raw = lst_parse(":nick!~user@server 305 circus-bot :Test message");

    irc_bind_event(RPL_UNAWAY, (CallbackPtr) on_dispatch);
    dsp_start();
    dsp_dispatch(raw);

    poll(0, 0, 1000);   /* Make sure the dispatcher thread process the event */

    dsp_shutdown();
    irc_unbind_event(RPL_UNAWAY);

    mu_assert(evt_dispatch == 1, "test_dsp_dispatch: evt_dispatch should be '1'");
}

void test_fire_evt_nick() {
    struct raw_event* raw;

    irc_bind_event(NICK, (CallbackPtr) on_nick);
    raw = lst_parse("NICK test-nick");
    _fire_event(raw);

    mu_assert(evt_nicks == 1, "test_fire_evt_nick: evt_nicks should be '1'");

    irc_unbind_event(NICK);
    evt_raw_destroy(raw);
}

void test_fire_evt_quit() {
    struct raw_event* raw;

    irc_bind_event(QUIT, (CallbackPtr) on_quit);
    raw = lst_parse("QUIT :Bye bye!");
    _fire_event(raw);

    mu_assert(evt_quits == 1, "test_fire_evt_quit: evt_quits should be '1'");

    irc_unbind_event(QUIT);
    evt_raw_destroy(raw);
}

void test_fire_evt_join() {
    struct raw_event* raw;

    irc_bind_event(JOIN, (CallbackPtr) on_join);
    raw = lst_parse("JOIN #circus");
    _fire_event(raw);

    mu_assert(evt_joins == 1, "test_fire_evt_join: evt_joins should be '1'");

    irc_unbind_event(JOIN);
    evt_raw_destroy(raw);
}

void test_fire_evt_part() {
    struct raw_event* raw;

    irc_bind_event(PART, (CallbackPtr) on_part);
    raw = lst_parse("PART #circus :Bye");
    _fire_event(raw);

    mu_assert(evt_parts == 1, "test_fire_evt_part: evt_parts should be '1'");

    irc_unbind_event(PART);
    evt_raw_destroy(raw);
}

void test_fire_evt_topic() {
    struct raw_event* raw;

    irc_bind_event(TOPIC, (CallbackPtr) on_topic);
    raw = lst_parse("TOPIC #circus :New topic");
    _fire_event(raw);

    mu_assert(evt_topics == 1, "test_fire_evt_topic: evt_topics should be '1'");

    irc_unbind_event(TOPIC);
    evt_raw_destroy(raw);
}

void test_fire_evt_names() {
    struct raw_event* raw;

    irc_bind_event(NAMES, (CallbackPtr) on_names);

    raw = lst_parse("353 test-nick @ #circus :test1 test2");
    _fire_event(raw);
    mu_assert(evt_namess == 1, "test_fire_evt_names: evt_namess should be '1'");
    evt_raw_destroy(raw);

    raw = lst_parse("366 test-nick #circus :End of /NAMES list");
    _fire_event(raw);
    mu_assert(evt_namess == 2, "test_fire_evt_names: evt_namess should be '2'");
    evt_raw_destroy(raw);

    irc_unbind_event(NAMES);
}

void test_fire_evt_list() {
    struct raw_event* raw;

    irc_bind_event(LIST, (CallbackPtr) on_list);

    raw = lst_parse(":moorcock.freenode.net 322 circus-bot #circus 7 :Circus IRC framework");
    _fire_event(raw);
    mu_assert(evt_lists == 1, "test_fire_evt_list: evt_lists should be '1'");
    evt_raw_destroy(raw);

    raw = lst_parse(":moorcock.freenode.net 323 circus-bot :End of /LIST");
    _fire_event(raw);
    mu_assert(evt_lists == 2, "test_fire_evt_list: evt_lists should be '2'");
    evt_raw_destroy(raw);

    irc_unbind_event(LIST);
}

void test_fire_evt_invite() {
    struct raw_event* raw;

    irc_bind_event(INVITE, (CallbackPtr) on_invite);
    raw = lst_parse(":nacx!~nacx@127.0.0.1 INVITE circus-bot :#circus");
    _fire_event(raw);

    mu_assert(evt_invites == 1, "test_fire_evt_invite: evt_invites should be '1'");

    irc_unbind_event(INVITE);
    evt_raw_destroy(raw);
}

void test_fire_evt_kick() {
    struct raw_event* raw;

    irc_bind_event(KICK, (CallbackPtr) on_kick);
    raw = lst_parse(":nacx!~nacx@127.0.0.1 KICK #circus circus-bot :Foo");
    _fire_event(raw);

    mu_assert(evt_kicks == 1, "test_fire_evt_kick: evt_kicks should be '1'");

    irc_unbind_event(KICK);
    evt_raw_destroy(raw);
}

void test_fire_evt_message() {
    struct raw_event* raw;

    irc_bind_event(PRIVMSG, (CallbackPtr) on_message);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :Hi there");
    _fire_event(raw);
    mu_assert(evt_messages == 1, "test_fire_evt_message: evt_messages should be '1'");
    evt_raw_destroy(raw);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :Hi there");
    _fire_event(raw);
    mu_assert(evt_messages == 2, "test_fire_evt_message: evt_messages should be '2'");
    evt_raw_destroy(raw);

    irc_unbind_event(PRIVMSG);
    irc_bind_command("!cmd", (CallbackPtr) on_message);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :!cmd Do it");
    _fire_event(raw);
    mu_assert(evt_messages == 3, "test_fire_evt_message: evt_messages should be '3'");
    evt_raw_destroy(raw);

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :!cmd");
    _fire_event(raw);
    mu_assert(evt_messages == 4, "test_fire_evt_message: evt_messages should be '2'");
    evt_raw_destroy(raw);

    irc_unbind_command("!cmd");
}

void test_fire_evt_mode() {
    struct raw_event* raw;

    irc_bind_event(MODE, (CallbackPtr) on_mode);
    raw = lst_parse(":nick!~user@server MODE #test +inm");
    _fire_event(raw);

    mu_assert(evt_modes == 1, "test_fire_evt_mode: evt_modes should be '1'");

    irc_unbind_event(MODE);
    evt_raw_destroy(raw);
}

void test_fire_evt_ping() {
    struct raw_event* raw;

    irc_bind_event(PING, (CallbackPtr) on_ping);
    raw = lst_parse("PING :zelazny.freenode.net");
    _fire_event(raw);

    mu_assert(evt_pings == 1, "test_fire_evt_ping: evt_pings should be '1'");

    irc_unbind_event(PING);
    evt_raw_destroy(raw);
}

void test_fire_evt_notice() {
    struct raw_event* raw;

    irc_bind_event(NOTICE, (CallbackPtr) on_notice);
    raw = lst_parse(":moorcock.freenode.net NOTICE * :Message");
    _fire_event(raw);

    mu_assert(evt_notices == 1, "test_fire_evt_notice: evt_notices should be '1'");

    irc_unbind_event(NOTICE);
    evt_raw_destroy(raw);
}

void test_fire_evt_error() {
    struct raw_event* raw;

    raw = lst_parse(":nick!~user@server 401 circus-bot :Test message");

    irc_bind_event(ERR_NOSUCHNICK, (CallbackPtr) on_error);
    _fire_event(raw);
    mu_assert(evt_errors == 1, "test_fire_evt_error: evt_errors should be '1'");

    irc_unbind_event(ERR_NOSUCHNICK);
    irc_bind_event(ERROR, (CallbackPtr) on_error);
    _fire_event(raw);
    mu_assert(evt_errors == 2, "test_fire_evt_error: evt_errors should be '2'");

    evt_raw_destroy(raw);
}

void test_fire_evt_generic() {
    struct raw_event* raw;

    raw = lst_parse(":nick!~user@server 305 circus-bot :Test message");

    irc_bind_event(RPL_UNAWAY, (CallbackPtr) on_generic);
    _fire_event(raw);
    mu_assert(evt_generics == 1, "test_fire_evt_generic: evt_generics should be '1'");

    irc_unbind_event(RPL_UNAWAY);
    irc_bind_event(ALL, (CallbackPtr) on_generic);
    _fire_event(raw);
    mu_assert(evt_generics == 2, "test_fire_evt_generic: evt_generics should be '2'");

    evt_raw_destroy(raw);
}

void test_dispatcher() {
    mu_run(test_queue_create_destroy);
    mu_run(test_queue_push_single);
    mu_run(test_queue_push_many);
    mu_run(test_queue_pop_single);
    mu_run(test_queue_pop_many);
    mu_run(test_consumer_create_destroy);
    mu_run(test_dsp_start_shutdown);
    mu_run(test_dsp_dispatch);

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

