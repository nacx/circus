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

#include <signal.h>
#include "minunit.h"
#include "test.h"
#include "../lib/binding.h"
#include "../lib/irc.c"


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

void test_irc() {
    mu_run(test_shutdown_handler);
    mu_run(test_bind_event);
    mu_run(test_unbind_event);
    mu_run(test_bind_command);
    mu_run(test_unbind_command);
}

