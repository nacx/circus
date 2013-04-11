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

#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "test.h"
#include "../lib/utils.h"
#include "../lib/listener.h"
#include "../lib/irc.h"
#include "../lib/events.h"


void test_user_info() {
    char user_ref[] = "nickname!~username@servername";
    UserInfo ui = user_info(user_ref);

    mu_assert(s_eq(ui.nick, "nickname"), "test_get_user_info: Nick should be 'nickname'");
    mu_assert(s_eq(ui.user, "username"), "test_get_user_info: User should be 'username'");
    mu_assert(s_eq(ui.server, "servername"), "test_get_user_info: Server should be 'servername'");
}

void test_evt_error_one_param() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nick!~user@server 401 circus-bot :Test message", buffer);
    event = evt_error(&raw);

    mu_assert(event.raw != NULL, "test_evt_error_one_param: raw message should not be NULL");
    mu_assert(s_eq(event.code, "401"), "test_evt_error_one_param: code should be 401");
    mu_assert(event.num_params == 1, "test_evt_error_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_evt_error_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_evt_error_one_param: message should be 'Test message'");

    free(buffer);   /* Cleanup */
}

void test_evt_error_no_params() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nick!~user@server 401 :Test message", buffer);
    event = evt_error(&raw);

    mu_assert(event.raw != NULL, "test_evt_error_no_params: raw message should not be NULL");
    mu_assert(s_eq(event.code, "401"), "test_evt_error_no_params: code should be 401");
    mu_assert(event.num_params == 0, "test_evt_error_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_evt_error_no_params: message should be 'Test message'");

    free(buffer);   /* Cleanup */
}

void test_evt_generic_one_param() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nick!~user@server 305 circus-bot :Test message", buffer);
    event = evt_generic(&raw);

    mu_assert(event.raw != NULL, "test_evt_generic_one_param: raw message should not be NULL");
    mu_assert(s_eq(event.code, "305"), "test_evt_generic_one_param: code should be 305");
    mu_assert(event.num_params == 1, "test_evt_generic_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_evt_generic_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_evt_generic_one_param: message should be 'test message'");

    free(buffer);   /* Cleanup */
}

void test_evt_generic_no_params() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nick!~user@server 305 :Test message", buffer);
    event = evt_generic(&raw);

    mu_assert(event.raw != NULL, "test_evt_generic_no_params: raw message should not be NULL");
    mu_assert(s_eq(event.code, "305"), "test_evt_generic_no_params: code should be 305");
    mu_assert(event.num_params == 0, "test_evt_generic_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_evt_generic_no_params: message should be 'test message'");

    free(buffer);   /* Cleanup */
}

void test_evt_nick() {
    NickEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("NICK test-nick", buffer);
    event = evt_nick(&raw);

    mu_assert(event.raw != NULL, "test_evt_nick: raw message should not be NULL");
    mu_assert(s_eq(event.new_nick, "test-nick"), "test_evt_nick: nick should be 'test-nick'");

    free(buffer);   /* Cleanup */
}

void test_evt_quit() {
    QuitEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("QUIT :Bye bye!", buffer);
    event = evt_quit(&raw);

    mu_assert(event.raw != NULL, "test_evt_quit: raw message should not be NULL");
    mu_assert(s_eq(event.message, "Bye bye!"), "test_evt_quit: message should be 'Bye bye!'");

    free(buffer);   /* Cleanup */
}

void test_evt_join() {
    JoinEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("JOIN #circus", buffer);
    event = evt_join(&raw);

    mu_assert(event.raw != NULL, "test_evt_join: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_join: channel should be '#circus'");

    free(buffer);   /* Cleanup */
}

void test_evt_part() {
    PartEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("PART #circus :Bye", buffer);
    event = evt_part(&raw);

    mu_assert(event.raw != NULL, "test_evt_part: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_part: channel should be '#circus'");
    mu_assert(s_eq(event.message, "Bye"), "test_evt_part: message should be 'Bye'");

    free(buffer);   /* Cleanup */
}

void test_evt_names_partial() {
    NamesEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("353 test-nick @ #circus :test1 test2", buffer);
    event = evt_names(&raw);

    mu_assert(event.raw != NULL, "test_evt_names_partial: raw message should not be NULL");
    mu_assert(!event.finished, "test_evt_names_partial: event should not be finished");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_names_partial: channel should be '#circus'");
    mu_assert(event.num_names == 2, "test_evt_names_partial: num_names should be '2'");
    mu_assert(s_eq(event.names[0], "test1"), "test_evt_names_partial: event.names[0] should be 'test1'");
    mu_assert(s_eq(event.names[1], "test2"), "test_evt_names_partial: event.names[1] should be 'test2'");

    free(buffer);   /* Cleanup */
}

void test_evt_names_finished() {
    NamesEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("366 test-nick #circus :End of /NAMES list", buffer);
    event = evt_names(&raw);

    mu_assert(event.raw != NULL, "test_evt_names_finished: raw message should not be NULL");
    mu_assert(event.finished, "test_evt_names_finished: event should be finished");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_names_finished: channel should be '#circus'");
    mu_assert(event.num_names == 0, "test_evt_names_finished: num_names should be '0'");

    free(buffer);   /* Cleanup */
}

void test_evt_topic() {
    TopicEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("TOPIC #circus :New topic", buffer);
    event = evt_topic(&raw);

    mu_assert(event.raw != NULL, "test_evt_topic: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_topic: channel should be '#circus'");
    mu_assert(s_eq(event.topic, "New topic"), "test_evt_topic: topic should be 'New topic'");

    free(buffer);   /* Cleanup */
}

void test_evt_list_partial() {
    ListEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":moorcock.freenode.net 322 circus-bot #circus 7 :Circus IRC framework", buffer);
    event = evt_list(&raw);

    mu_assert(event.raw != NULL, "test_evt_list_partial: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_list_partial: channel should be '#circus'");
    mu_assert(!event.finished, "test_evt_list_partial: event should not be finished");
    mu_assert(event.num_users == 7, "test_evt_list_partial: num_users should be '7'");
    mu_assert(s_eq(event.topic, "Circus IRC framework"), "test_evt_list_partial: topic should be 'Circus IRC framework'");

    free(buffer);   /* Cleanup */
}

void test_evt_list_finished() {
    ListEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":moorcock.freenode.net 323 circus-bot :End of /LIST", buffer);
    event = evt_list(&raw);

    mu_assert(event.raw != NULL, "test_evt_list_finished: raw message should not be NULL");
    mu_assert(event.channel == NULL, "test_evt_list_finished: channel should be 'NULL'");
    mu_assert(event.finished, "test_evt_list_finished: event should be finished");
    mu_assert(event.num_users == 0, "test_evt_list_finished: num_users should be '0'");
    mu_assert(event.topic == NULL, "test_evt_list_finished: topic should be 'NULL'");

    free(buffer);   /* Cleanup */
}

void test_evt_invite() {
    InviteEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nacx!~nacx@127.0.0.1 INVITE circus-bot :#circus", buffer);
    event = evt_invite(&raw);

    mu_assert(event.raw != NULL, "test_evt_invite: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_invite: channel should be '#circus'");
    mu_assert(s_eq(event.nick, "circus-bot"), "test_evt_invite: nick should be 'circus-bot'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_evt_invite: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_evt_kick() {
    KickEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nacx!~nacx@127.0.0.1 KICK #circus circus-bot :Foo", buffer);
    event = evt_kick(&raw);

    mu_assert(event.raw != NULL, "test_evt_kick: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_evt_kick: channel should be '#circus'");
    mu_assert(s_eq(event.nick, "circus-bot"), "test_evt_kick: nick should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Foo"), "test_evt_kick: message should be 'Foo'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_evt_kick: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_evt_message_channel() {
    MessageEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :Hi there", buffer);
    event = evt_message(&raw);

    mu_assert(event.raw != NULL, "test_evt_message_channel: raw message should not be NULL");
    mu_assert(s_eq(event.to, "#circus"), "test_evt_message_channel: event.to should be '#circus'");
    mu_assert(event.is_channel, "test_evt_message_channel: event.is_channel should be 'true'");
    mu_assert(s_eq(event.message, "Hi there"), "test_evt_message_channel: message should be 'Hi there'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_evt_message_channel: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_evt_message_private() {
    MessageEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :Hi there", buffer);
    event = evt_message(&raw);

    mu_assert(event.raw != NULL, "test_evt_message_private: raw message should not be NULL");
    mu_assert(s_eq(event.to, "circus-bot"), "test_evt_message_channel: event.to should be 'circus-bot'");
    mu_assert(!event.is_channel, "test_evt_message_channel: event.is_channel should be 'false'");
    mu_assert(s_eq(event.message, "Hi there"), "test_evt_message_channel: message should be 'Hi there'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_evt_message_channel: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_evt_ping() {
    PingEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse("PING :zelazny.freenode.net", buffer);
    event = evt_ping(&raw);

    mu_assert(event.raw != NULL, "test_evt_ping: raw message should not be NULL");
    mu_assert(s_eq(event.server, "zelazny.freenode.net"), "test_evt_ping: server should be 'zelazny.freenode.net'");

    free(buffer);   /* Cleanup */
}

void test_evt_notice() {
    NoticeEvent event;
    char* buffer = NULL;
    struct raw_event raw;

    raw = lst_parse(":moorcock.freenode.net NOTICE * :Message", buffer);
    event = evt_notice(&raw);

    mu_assert(event.raw != NULL, "test_evt_notice: raw message should not be NULL");
    mu_assert(s_eq(event.to, "*"), "test_evt_notice: event.to should be '*'");
    mu_assert(s_eq(event.text, "Message"), "test_evt_notice: event.text should be 'Message'");

    free(buffer);   /* Cleanup */
}

void test_channel_evt_mode_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = lst_parse(":nick!~user@server MODE #test +inm", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_channel_evt_mode_set: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_evt_mode_set: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_evt_mode_set: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_evt_mode_set: set_flags should be 'inm'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_evt_mode_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_channel_evt_mode_set: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_evt_mode_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = lst_parse(":nick!~user@server MODE #test -inm", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_channel_evt_mode_unset: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_evt_mode_unset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_evt_mode_unset: target should be '#test'");
    mu_assert(event.set_flags == 0x0000, "test_channel_evt_mode_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_channel_evt_mode_unset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_evt_mode_unset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_evt_mode_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int set_flags = CH_TOPICLOCK | CH_SECRET;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = lst_parse(":nick!~user@server MODE #test -i+ts-nm", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_channel_evt_mode_setunset: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_evt_mode_setunset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_evt_mode_setunset: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_evt_mode_setunset: set_flags should be 'ts'");
    mu_assert(event.unset_flags == unset_flags, "test_channel_evt_mode_setunset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_evt_mode_setunset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_evt_mode_params() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_LIMIT | CH_BAN;

    raw = lst_parse(":nick!~user@server MODE #test +ilb 10 test *!*@*", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_channel_evt_mode_params: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_evt_mode_params: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_evt_mode_params: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_evt_mode_params: set_flags should be 'i'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_evt_mode_params: unset_flags should be 0x0000");
    mu_assert(event.num_params == 3, "test_channel_evt_mode_params: num_params should be 3");
    mu_assert(s_eq(event.params[0], "10"), "test_channel_evt_mode_params: params[0] should be '10'");
    mu_assert(s_eq(event.params[1], "test"), "test_channel_evt_mode_params: params[0] should be 'test'");
    mu_assert(s_eq(event.params[2], "*!*@*"), "test_channel_evt_mode_params: params[0] should be '*!*@*'");

    free(buffer);   /* Cleanup */
}

void test_user_evt_mode_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int set_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = lst_parse(":test MODE test +iw", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_user_evt_mode_set: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_evt_mode_set: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_evt_mode_set: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_evt_mode_set: set_flags should be 'iw'");
    mu_assert(event.unset_flags == 0x0000, "test_user_evt_mode_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_user_evt_mode_set: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_user_evt_mode_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int unset_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = lst_parse(":test MODE test -iw", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_user_evt_mode_unset: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_evt_mode_unset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_evt_mode_unset: target should be 'test'");
    mu_assert(event.set_flags == 0x0000, "test_user_evt_mode_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_user_evt_mode_unset: unset_flags should be 'ow'");
    mu_assert(event.num_params == 0, "test_user_evt_mode_unset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_user_evt_mode_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_event raw;
    unsigned short int set_flags = USR_WALLOPS;
    unsigned short int unset_flags = USR_INVISIBLE | USR_OPERATOR;

    raw = lst_parse(":test MODE test -i+w-o", buffer);
    event = evt_mode(&raw);

    mu_assert(event.raw != NULL, "test_user_evt_mode_setunset: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_evt_mode_setunset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_evt_mode_setunset: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_evt_mode_setunset: set_flags should be 'w'");
    mu_assert(event.unset_flags == unset_flags, "test_user_evt_mode_setunset: unset_flags should be 'io'");
    mu_assert(event.num_params == 0, "test_user_evt_mode_setunset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_events() {
    mu_run(test_user_info);
    mu_run(test_evt_error_one_param);
    mu_run(test_evt_error_no_params);
    mu_run(test_evt_generic_one_param);
    mu_run(test_evt_generic_no_params);
    mu_run(test_evt_nick);
    mu_run(test_evt_quit);
    mu_run(test_evt_join);
    mu_run(test_evt_part);
    mu_run(test_evt_names_partial);
    mu_run(test_evt_names_finished);
    mu_run(test_evt_topic);
    mu_run(test_evt_list_partial);
    mu_run(test_evt_list_finished);
    mu_run(test_evt_invite);
    mu_run(test_evt_kick);
    mu_run(test_evt_message_channel);
    mu_run(test_evt_message_private);
    mu_run(test_evt_ping);
    mu_run(test_evt_notice);
    mu_run(test_channel_evt_mode_set);
    mu_run(test_channel_evt_mode_unset);
    mu_run(test_channel_evt_mode_setunset);
    mu_run(test_channel_evt_mode_params);
    mu_run(test_user_evt_mode_set);
    mu_run(test_user_evt_mode_unset);
    mu_run(test_user_evt_mode_setunset);
}

