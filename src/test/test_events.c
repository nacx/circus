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
#include "../lib/events.h"


char* test_user_info() {
    char user_ref[] = "nickname!~username@servername";
    UserInfo ui = user_info(user_ref);

    mu_assert(s_eq(ui.nick, "nickname"), "test_get_user_info: Nick should be 'nickname'");
    mu_assert(s_eq(ui.user, "username"), "test_get_user_info: User should be 'username'");
    mu_assert(s_eq(ui.server, "servername"), "test_get_user_info: Server should be 'servername'");

    return 0;
}

char* test_error_event_one_param() {
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

    return 0;
}

char* test_error_event_no_params() {
    ErrorEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
   
    raw = parse(":nick!~user@server 401 :Test message", buffer);
    event = error_event(&raw);

    mu_assert(s_eq(event.code, "401"), "test_error_event_no_params: code should be 401");
    mu_assert(event.num_params == 0, "test_error_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_error_event_no_params: message should be 'Test message'");

    free(buffer);   // Cleanup

    return 0;
}

char* test_generic_event_one_param() {
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

    return 0;
}

char* test_generic_event_no_params() {
    GenericEvent event;
    char* buffer = NULL;
    struct raw_msg raw;
   
    raw = parse(":nick!~user@server 305 :Test message", buffer);
    event = generic_event(&raw);

    mu_assert(s_eq(event.code, "305"), "test_generic_event_no_params: code should be 305");
    mu_assert(event.num_params == 0, "test_generic_event_no_params: num_params should be 0");
    mu_assert(s_eq(event.message, "Test message"), "test_generic_event_no_params: message should be 'test message'");

    free(buffer);   // Cleanup

    return 0;
}

void test_events() {
    mu_run(test_user_info);
    mu_run(test_error_event_one_param);
    mu_run(test_error_event_no_params);
    mu_run(test_generic_event_one_param);
    mu_run(test_generic_event_no_params);
}

