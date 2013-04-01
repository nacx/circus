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

#include <string.h>
#include "minunit.h"
#include "test.h"
#include "../lib/irc.h"
#include "../lib/utils.h"

void test_upper() {
    char text[10] = "Test Text";
    upper(text);
    mu_assert(s_eq(text, "TEST TEXT"), "test_uppper: text should be 'TEST TEXT'");
}

void test_lower() {
    char text[10] = "Test Text";
    lower(text);
    mu_assert(s_eq(text, "test text"), "test_uppper: text should be 'test text'");
}

void test_build_command_key() {
    char key[50];

    build_command_key(key, "cmd");
    mu_assert(s_eq(key, "PRIVMSG#cmd"), "test_build_command_key: key should be 'PRIVMSG#cmd'");

    build_command_key(key, "");
    mu_assert(s_eq(key, "PRIVMSG#"), "test_build_command_key: key should be 'PRIVMSG#'");
}

void test_append_channel_flags() {
    char text[10] = "";

    /* No flags set */
    append_channel_flags(text, 0x0000);
    mu_assert(s_eq(text, ""), "test_append_channel_flags: text should be an empty string");

    /* Single flag */
    *text = '\0';
    append_channel_flags(text, CH_MODERATED);
    mu_assert(s_eq(text, "m"), "test_append_channel_flags: text should be 'm'");

    /* Multiple flags are set in the order defined in the append_channel_flags function */
    *text = '\0';
    append_channel_flags(text, CH_INVITEONLY | CH_NOEXTMSGS | CH_TOPICLOCK);
    mu_assert(s_eq(text, "itn"), "test_append_channel_flags: text should be 'itn'");

    /* All flags */
    *text = '\0';
    append_channel_flags(text, 0xFFFF);
    mu_assert(s_eq(text, "psitnm"), "test_append_channel_flags: text should be 'psitnm'");
}

void test_append_user_flags() {
    char text[10] = "";

    /* No flags set */
    append_user_flags(text, 0x0000);
    mu_assert(s_eq(text, ""), "test_append_user_flags: text should be an empty string");

    /* Single flag */
    *text = '\0';
    append_user_flags(text, USR_INVISIBLE);
    mu_assert(s_eq(text, "i"), "test_append_user_flags: text should be 'i'");

    /* Multiple flags are set in the order defined in the append_channel_flags function */
    *text = '\0';
    append_user_flags(text, USR_WALLOPS | USR_INVISIBLE);
    mu_assert(s_eq(text, "iw"), "test_append_user_flags: text should be 'iw'");

    /* All flags */
    *text = '\0';
    append_user_flags(text, 0xFFFF);
    mu_assert(s_eq(text, "iwo"), "test_append_user_flags: text should be 'iwo'");
}

void test_utils() {
    mu_run(test_upper);
    mu_run(test_lower);
    mu_run(test_build_command_key);
    mu_run(test_append_channel_flags);
    mu_run(test_append_user_flags);
}

