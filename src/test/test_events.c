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
#include "../lib/message_handler.h"
#include "../lib/irc.h"
#include "../lib/events.h"


void test_user_info() {
    char user_ref[] = "nickname!~username@servername";
    UserInfo ui = user_info(user_ref);

    mu_assert(s_eq(ui.nick, "nickname"), "test_get_user_info: Nick should be 'nickname'");
    mu_assert(s_eq(ui.user, "username"), "test_get_user_info: User should be 'username'");
    mu_assert(s_eq(ui.server, "servername"), "test_get_user_info: Server should be 'servername'");
}

void test_error_event_one_param() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 401 circus-bot :Test message", buffer);
    event = error_event(&raw);

    mu_assert(event.raw != NULL, "test_error_event_one_param: raw message should not be NULL");
    mu_assert(s_eq(event.code, "401"), "test_error_event_one_param: code should be 401");
    mu_assert(event.num_params == 1, "test_error_event_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_error_event_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_error_event_one_param: message should be 'Test message'");

    free(buffer);   /* Cleanup */
}

void test_error_event_no_params() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 401 :Test message", buffer);
    event = error_event(&raw);

    mu_assert(event.raw != NULL, "test_error_event_no_params: raw message should not be NULL");
    mu_assert(s_eq(event.code, "401"), "test_error_event_no_params: code should be 401");
    mu_assert(event.num_params == 0, "test_error_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_error_event_no_params: message should be 'Test message'");

    free(buffer);   /* Cleanup */
}

void test_generic_event_one_param() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 305 circus-bot :Test message", buffer);
    event = generic_event(&raw);

    mu_assert(event.raw != NULL, "test_generic_event_one_param: raw message should not be NULL");
    mu_assert(s_eq(event.code, "305"), "test_generic_event_one_param: code should be 305");
    mu_assert(event.num_params == 1, "test_generic_event_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_generic_event_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_generic_event_one_param: message should be 'test message'");

    free(buffer);   /* Cleanup */
}

void test_generic_event_no_params() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 305 :Test message", buffer);
    event = generic_event(&raw);

    mu_assert(event.raw != NULL, "test_generic_event_no_params: raw message should not be NULL");
    mu_assert(s_eq(event.code, "305"), "test_generic_event_no_params: code should be 305");
    mu_assert(event.num_params == 0, "test_generic_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_generic_event_no_params: message should be 'test message'");

    free(buffer);   /* Cleanup */
}

void test_nick_event() {
    NickEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("NICK test-nick", buffer);
    event = nick_event(&raw);

    mu_assert(event.raw != NULL, "test_nick_event: raw message should not be NULL");
    mu_assert(s_eq(event.new_nick, "test-nick"), "test_nick_event: nick should be 'test-nick'");

    free(buffer);   /* Cleanup */
}

void test_quit_event() {
    QuitEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("QUIT :Bye bye!", buffer);
    event = quit_event(&raw);

    mu_assert(event.raw != NULL, "test_quit_event: raw message should not be NULL");
    mu_assert(s_eq(event.message, "Bye bye!"), "test_quit_event: message should be 'Bye bye!'");

    free(buffer);   /* Cleanup */
}

void test_join_event() {
    JoinEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("JOIN #circus", buffer);
    event = join_event(&raw);

    mu_assert(event.raw != NULL, "test_join_event: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_join_event: channel should be '#circus'");

    free(buffer);   /* Cleanup */
}

void test_part_event() {
    PartEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("PART #circus :Bye", buffer);
    event = part_event(&raw);

    mu_assert(event.raw != NULL, "test_part_event: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_part_event: channel should be '#circus'");
    mu_assert(s_eq(event.message, "Bye"), "test_part_event: message should be 'Bye'");

    free(buffer);   /* Cleanup */
}

void test_names_event_partial() {
    NamesEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("353 test-nick @ #circus :test1 test2", buffer);
    event = names_event(&raw);

    mu_assert(event.raw != NULL, "test_names_event_partial: raw message should not be NULL");
    mu_assert(!event.finished, "test_names_event_partial: event should not be finished");
    mu_assert(s_eq(event.channel, "#circus"), "test_names_event_partial: channel should be '#circus'");
    mu_assert(event.num_names == 2, "test_names_event_partial: num_names should be '2'");
    mu_assert(s_eq(event.names[0], "test1"), "test_names_event_partial: event.names[0] should be 'test1'");
    mu_assert(s_eq(event.names[1], "test2"), "test_names_event_partial: event.names[1] should be 'test2'");

    free(buffer);   /* Cleanup */
}

void test_names_event_finished() {
    NamesEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("366 test-nick #circus :End of /NAMES list", buffer);
    event = names_event(&raw);

    mu_assert(event.raw != NULL, "test_names_event_finished: raw message should not be NULL");
    mu_assert(event.finished, "test_names_event_finished: event should be finished");
    mu_assert(s_eq(event.channel, "#circus"), "test_names_event_finished: channel should be '#circus'");
    mu_assert(event.num_names == 0, "test_names_event_finished: num_names should be '0'");

    free(buffer);   /* Cleanup */
}

void test_topic_event() {
    TopicEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("TOPIC #circus :New topic", buffer);
    event = topic_event(&raw);

    mu_assert(event.raw != NULL, "test_topic_event: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_topic_event: channel should be '#circus'");
    mu_assert(s_eq(event.topic, "New topic"), "test_topic_event: topic should be 'New topic'");

    free(buffer);   /* Cleanup */
}

void test_list_event_partial() {
    ListEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":moorcock.freenode.net 322 circus-bot #circus 7 :Circus IRC framework", buffer);
    event = list_event(&raw);

    mu_assert(event.raw != NULL, "test_list_event_partial: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_list_event_partial: channel should be '#circus'");
    mu_assert(!event.finished, "test_list_event_partial: event should not be finished");
    mu_assert(event.num_users == 7, "test_list_event_partial: num_users should be '7'");
    mu_assert(s_eq(event.topic, "Circus IRC framework"), "test_list_event_partial: topic should be 'Circus IRC framework'");

    free(buffer);   /* Cleanup */
}

void test_list_event_finished() {
    ListEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":moorcock.freenode.net 323 circus-bot :End of /LIST", buffer);
    event = list_event(&raw);

    mu_assert(event.raw != NULL, "test_list_event_finished: raw message should not be NULL");
    mu_assert(event.channel == NULL, "test_list_event_finished: channel should be 'NULL'");
    mu_assert(event.finished, "test_list_event_finished: event should be finished");
    mu_assert(event.num_users == 0, "test_list_event_finished: num_users should be '0'");
    mu_assert(event.topic == NULL, "test_list_event_finished: topic should be 'NULL'");

    free(buffer);   /* Cleanup */
}

void test_invite_event() {
    InviteEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nacx!~nacx@127.0.0.1 INVITE circus-bot :#circus", buffer);
    event = invite_event(&raw);

    mu_assert(event.raw != NULL, "test_invite_event: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_invite_event: channel should be '#circus'");
    mu_assert(s_eq(event.nick, "circus-bot"), "test_invite_event: nick should be 'circus-bot'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_invite_event: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_kick_event() {
    KickEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nacx!~nacx@127.0.0.1 KICK #circus circus-bot :Foo", buffer);
    event = kick_event(&raw);

    mu_assert(event.raw != NULL, "test_kick_event: raw message should not be NULL");
    mu_assert(s_eq(event.channel, "#circus"), "test_kick_event: channel should be '#circus'");
    mu_assert(s_eq(event.nick, "circus-bot"), "test_kick_event: nick should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Foo"), "test_kick_event: message should be 'Foo'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_kick_event: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_message_event_channel() {
    MessageEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG #circus :Hi there", buffer);
    event = message_event(&raw);

    mu_assert(event.raw != NULL, "test_message_event_channel: raw message should not be NULL");
    mu_assert(s_eq(event.to, "#circus"), "test_message_event_channel: event.to should be '#circus'");
    mu_assert(event.is_channel, "test_message_event_channel: event.is_channel should be 'true'");
    mu_assert(s_eq(event.message, "Hi there"), "test_message_event_channel: message should be 'Hi there'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_message_event_channel: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_message_event_private() {
    MessageEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nacx!~nacx@127.0.0.1 PRIVMSG circus-bot :Hi there", buffer);
    event = message_event(&raw);

    mu_assert(event.raw != NULL, "test_message_event_private: raw message should not be NULL");
    mu_assert(s_eq(event.to, "circus-bot"), "test_message_event_channel: event.to should be 'circus-bot'");
    mu_assert(!event.is_channel, "test_message_event_channel: event.is_channel should be 'false'");
    mu_assert(s_eq(event.message, "Hi there"), "test_message_event_channel: message should be 'Hi there'");
    mu_assert(s_eq(event.user.nick, "nacx"), "test_message_event_channel: user.nick should be 'nacx'");

    free(buffer);   /* Cleanup */
}

void test_ping_event() {
    PingEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse("PING :zelazny.freenode.net", buffer);
    event = ping_event(&raw);

    mu_assert(event.raw != NULL, "test_ping_event: raw message should not be NULL");
    mu_assert(s_eq(event.server, "zelazny.freenode.net"), "test_ping_event: server should be 'zelazny.freenode.net'");

    free(buffer);   /* Cleanup */
}

void test_notice_event() {
    NoticeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":moorcock.freenode.net NOTICE * :Message", buffer);
    event = notice_event(&raw);

    mu_assert(event.raw != NULL, "test_notice_event: raw message should not be NULL");
    mu_assert(s_eq(event.to, "*"), "test_notice_event: event.to should be '*'");
    mu_assert(s_eq(event.text, "Message"), "test_notice_event: event.text should be 'Message'");

    free(buffer);   /* Cleanup */
}

void test_channel_mode_event_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test +inm", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_channel_mode_event_set: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_mode_event_set: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_set: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_set: set_flags should be 'inm'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_mode_event_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_channel_mode_event_set: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_mode_event_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test -inm", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_channel_mode_event_unset: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_mode_event_unset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_unset: target should be '#test'");
    mu_assert(event.set_flags == 0x0000, "test_channel_mode_event_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_channel_mode_event_unset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_mode_event_unset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_mode_event_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_TOPICLOCK | CH_SECRET;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test -i+ts-nm", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_channel_mode_event_setunset: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_mode_event_setunset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_setunset: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_setunset: set_flags should be 'ts'");
    mu_assert(event.unset_flags == unset_flags, "test_channel_mode_event_setunset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_mode_event_setunset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_channel_mode_event_params() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_LIMIT | CH_BAN;

    raw = parse(":nick!~user@server MODE #test +ilb 10 test *!*@*", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_channel_mode_event_params: raw message should not be NULL");
    mu_assert(event.is_channel, "test_channel_mode_event_params: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_params: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_params: set_flags should be 'i'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_mode_event_params: unset_flags should be 0x0000");
    mu_assert(event.num_params == 3, "test_channel_mode_event_params: num_params should be 3");
    mu_assert(s_eq(event.params[0], "10"), "test_channel_mode_event_params: params[0] should be '10'");
    mu_assert(s_eq(event.params[1], "test"), "test_channel_mode_event_params: params[0] should be 'test'");
    mu_assert(s_eq(event.params[2], "*!*@*"), "test_channel_mode_event_params: params[0] should be '*!*@*'");

    free(buffer);   /* Cleanup */
}

void test_user_mode_event_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = parse(":test MODE test +iw", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_user_mode_event_set: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_mode_event_set: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_set: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_mode_event_set: set_flags should be 'iw'");
    mu_assert(event.unset_flags == 0x0000, "test_user_mode_event_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_user_mode_event_set: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_user_mode_event_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int unset_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = parse(":test MODE test -iw", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_user_mode_event_unset: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_mode_event_unset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_unset: target should be 'test'");
    mu_assert(event.set_flags == 0x0000, "test_user_mode_event_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_user_mode_event_unset: unset_flags should be 'ow'");
    mu_assert(event.num_params == 0, "test_user_mode_event_unset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_user_mode_event_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = USR_WALLOPS;
    unsigned short int unset_flags = USR_INVISIBLE | USR_OPERATOR;

    raw = parse(":test MODE test -i+w-o", buffer);
    event = mode_event(&raw);

    mu_assert(event.raw != NULL, "test_user_mode_event_setunset: raw message should not be NULL");
    mu_assert(!event.is_channel, "test_user_mode_event_setunset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_setunset: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_mode_event_setunset: set_flags should be 'w'");
    mu_assert(event.unset_flags == unset_flags, "test_user_mode_event_setunset: unset_flags should be 'io'");
    mu_assert(event.num_params == 0, "test_user_mode_event_setunset: num_params should be 0");

    free(buffer);   /* Cleanup */
}

void test_events() {
    mu_run(test_user_info);
    mu_run(test_error_event_one_param);
    mu_run(test_error_event_no_params);
    mu_run(test_generic_event_one_param);
    mu_run(test_generic_event_no_params);
    mu_run(test_nick_event);
    mu_run(test_quit_event);
    mu_run(test_join_event);
    mu_run(test_part_event);
    mu_run(test_names_event_partial);
    mu_run(test_names_event_finished);
    mu_run(test_topic_event);
    mu_run(test_list_event_partial);
    mu_run(test_list_event_finished);
    mu_run(test_invite_event);
    mu_run(test_kick_event);
    mu_run(test_message_event_channel);
    mu_run(test_message_event_private);
    mu_run(test_ping_event);
    mu_run(test_notice_event);
    mu_run(test_channel_mode_event_set);
    mu_run(test_channel_mode_event_unset);
    mu_run(test_channel_mode_event_setunset);
    mu_run(test_channel_mode_event_params);
    mu_run(test_user_mode_event_set);
    mu_run(test_user_mode_event_unset);
    mu_run(test_user_mode_event_setunset);
}

