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

    mu_assert(s_eq(event.code, "401"), "test_error_event_one_param: code should be 401");
    mu_assert(event.num_params == 1, "test_error_event_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_error_event_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_error_event_one_param: message should be 'Test message'");

    free(buffer);   // Cleanup
}

void test_error_event_no_params() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 401 :Test message", buffer);
    event = error_event(&raw);

    mu_assert(s_eq(event.code, "401"), "test_error_event_no_params: code should be 401");
    mu_assert(event.num_params == 0, "test_error_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_error_event_no_params: message should be 'Test message'");

    free(buffer);   // Cleanup
}

void test_generic_event_one_param() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 305 circus-bot :Test message", buffer);
    event = generic_event(&raw);

    mu_assert(s_eq(event.code, "305"), "test_generic_event_one_param: code should be 305");
    mu_assert(event.num_params == 1, "test_generic_event_one_param: num_params should be 1");
    mu_assert(s_eq(event.params[0], "circus-bot"), "test_generic_event_one_param: params[0] should be 'circus-bot'");
    mu_assert(s_eq(event.message, "Test message"), "test_generic_event_one_param: message should be 'test message'");

    free(buffer);   // Cleanup
}

void test_generic_event_no_params() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_msg raw;

    raw = parse(":nick!~user@server 305 :Test message", buffer);
    event = generic_event(&raw);

    mu_assert(s_eq(event.code, "305"), "test_generic_event_no_params: code should be 305");
    mu_assert(event.num_params == 0, "test_generic_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_generic_event_no_params: message should be 'test message'");

    free(buffer);   // Cleanup
}

void test_channel_mode_event_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test +inm", buffer);
    event = mode_event(&raw);

    mu_assert(event.is_channel, "test_channel_mode_event_set: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_set: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_set: set_flags should be 'inm'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_mode_event_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_channel_mode_event_set: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_channel_mode_event_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test -inm", buffer);
    event = mode_event(&raw);

    mu_assert(event.is_channel, "test_channel_mode_event_unset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_unset: target should be '#test'");
    mu_assert(event.set_flags == 0x0000, "test_channel_mode_event_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_channel_mode_event_unset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_mode_event_unset: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_channel_mode_event_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_TOPICLOCK | CH_SECRET;
    unsigned short int unset_flags = CH_INVITEONLY | CH_NOEXTMSGS | CH_MODERATED;

    raw = parse(":nick!~user@server MODE #test -i+ts-nm", buffer);
    event = mode_event(&raw);

    mu_assert(event.is_channel, "test_channel_mode_event_setunset: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_setunset: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_setunset: set_flags should be 'ts'");
    mu_assert(event.unset_flags == unset_flags, "test_channel_mode_event_setunset: unset_flags should be 'inm'");
    mu_assert(event.num_params == 0, "test_channel_mode_event_setunset: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_channel_mode_event_params() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = CH_INVITEONLY | CH_LIMIT | CH_BAN;

    raw = parse(":nick!~user@server MODE #test +ilb 10 test *!*@*", buffer);
    event = mode_event(&raw);

    mu_assert(event.is_channel, "test_channel_mode_event_params: is_channel should be true");
    mu_assert(s_eq(event.target, "#test"), "test_channel_mode_event_params: target should be '#test'");
    mu_assert(event.set_flags == set_flags, "test_channel_mode_event_params: set_flags should be 'i'");
    mu_assert(event.unset_flags == 0x0000, "test_channel_mode_event_params: unset_flags should be 0x0000");
    mu_assert(event.num_params == 3, "test_channel_mode_event_params: num_params should be 3");
    mu_assert(s_eq(event.params[0], "10"), "test_channel_mode_event_params: params[0] should be '10'");
    mu_assert(s_eq(event.params[1], "test"), "test_channel_mode_event_params: params[0] should be 'test'");
    mu_assert(s_eq(event.params[2], "*!*@*"), "test_channel_mode_event_params: params[0] should be '*!*@*'");

    free(buffer);   // Cleanup
}

void test_user_mode_event_set() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = parse(":test MODE test +iw", buffer);
    event = mode_event(&raw);

    mu_assert(!event.is_channel, "test_user_mode_event_set: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_set: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_mode_event_set: set_flags should be 'iw'");
    mu_assert(event.unset_flags == 0x0000, "test_user_mode_event_set: unset_flags should be 0x0000");
    mu_assert(event.num_params == 0, "test_user_mode_event_set: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_user_mode_event_unset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int unset_flags = USR_INVISIBLE | USR_WALLOPS;

    raw = parse(":test MODE test -iw", buffer);
    event = mode_event(&raw);

    mu_assert(!event.is_channel, "test_user_mode_event_unset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_unset: target should be 'test'");
    mu_assert(event.set_flags == 0x0000, "test_user_mode_event_unset: set_flags should be 0x0000");
    mu_assert(event.unset_flags == unset_flags, "test_user_mode_event_unset: unset_flags should be 'ow'");
    mu_assert(event.num_params == 0, "test_user_mode_event_unset: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_user_mode_event_setunset() {
    ModeEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
    unsigned short int set_flags = USR_WALLOPS;
    unsigned short int unset_flags = USR_INVISIBLE | USR_OPERATOR;

    raw = parse(":test MODE test -i+w-o", buffer);
    event = mode_event(&raw);

    mu_assert(!event.is_channel, "test_user_mode_event_setunset: is_channel should be false");
    mu_assert(s_eq(event.target, "test"), "test_user_mode_event_setunset: target should be 'test'");
    mu_assert(event.set_flags == set_flags, "test_user_mode_event_setunset: set_flags should be 'w'");
    mu_assert(event.unset_flags == unset_flags, "test_user_mode_event_setunset: unset_flags should be 'io'");
    mu_assert(event.num_params == 0, "test_user_mode_event_setunset: num_params should be 0");

    free(buffer);   // Cleanup
}

void test_events() {
    mu_run(test_user_info);
    mu_run(test_error_event_one_param);
    mu_run(test_error_event_no_params);
    mu_run(test_generic_event_one_param);
    mu_run(test_generic_event_no_params);
    mu_run(test_channel_mode_event_set);
    mu_run(test_channel_mode_event_unset);
    mu_run(test_channel_mode_event_setunset);
    mu_run(test_channel_mode_event_params);
    mu_run(test_user_mode_event_set);
    mu_run(test_user_mode_event_unset);
    mu_run(test_user_mode_event_setunset);
}

