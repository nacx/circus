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
#include "../lib/listener.c"


void test_parse_empty_message() {
    struct raw_event* raw;

    raw = lst_parse(NULL);

    mu_assert(raw->prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw->type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw->num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    evt_raw_destroy(raw);   /* Cleanup */

    raw = lst_parse("");

    mu_assert(raw->prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw->type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw->num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse() {
    struct raw_event* raw;

    raw = lst_parse("TEST This is a message test without prefix");

    mu_assert(raw->prefix == NULL, "test_parse: prefix should be NULL"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse: type should be TEST"); 
    mu_assert(raw->num_params == 7, "test_parse: there should be 7 parameters");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse_with_prefix() {
    struct raw_event* raw;

    raw = lst_parse(":prefix TEST This is a message test with prefix");

    mu_assert(s_eq(raw->prefix, "prefix"), "test_parse_with_prefix: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse_with_prefix: type should be 'TEST'"); 
    mu_assert(raw->num_params == 7, "test_parse_with_prefix: there should be 7 parameters");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse_with_last_param() {
    char* last_param;
    struct raw_event* raw;

    raw = lst_parse("TEST This is a message test with a :last parameter");
    last_param = raw->params[raw->num_params -1];

    mu_assert(raw->prefix == NULL, "test_parse_with_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse_with_last_param: type should be 'TEST'"); 
    mu_assert(raw->num_params == 8, "test_parse_with_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_last_param: last parameter should be 'last parameter'");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse_with_prefix_and_last_param() {
    char* last_param;
    struct raw_event* raw;

    raw = lst_parse(":prefix TEST This is a message test with a :last parameter");
    last_param = raw->params[raw->num_params - 1];

    mu_assert(s_eq(raw->prefix, "prefix"), "test_parse_with_prefix_and_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse_with_prefix_and_last_param: type should be 'TEST'"); 
    mu_assert(raw->num_params == 8, "test_parse_with_prefix_and_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_prefix_and_last_param: last parameter should be 'last parameter'");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse_only_last_param() {
    char* last_param;
    struct raw_event* raw;

    raw = lst_parse("TEST :only last parameter");
    last_param = raw->params[raw->num_params -1];

    mu_assert(raw->prefix == NULL, "test_parse_only_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse_only_last_param: type should be 'TEST'"); 
    mu_assert(raw->num_params == 1, "test_parse_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_only_last_param: last parameter should be 'only last parameter'");

    evt_raw_destroy(raw);   /* Cleanup */
}

void test_parse_with_prefix_only_last_param() {
    char* last_param;
    struct raw_event* raw;

    raw = lst_parse(":prefix TEST :only last parameter");
    last_param = raw->params[raw->num_params - 1];

    mu_assert(s_eq(raw->prefix, "prefix"), "test_parse_with_prefix_only_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw->type, "TEST"), "test_parse_with_prefix_only_last_param: type should be 'TEST'"); 
    mu_assert(raw->num_params == 1, "test_parse_with_prefix_only_last_param: there should be 1 parameters");
    mu_assert(s_eq(last_param, "only last parameter"), "test_parse_with_prefix_only_last_param: last parameter should be 'only last parameter'");

    evt_raw_destroy(raw);   /* Cleanup */
}



void test_listener() {
    mu_run(test_parse_empty_message);
    mu_run(test_parse);
    mu_run(test_parse_with_prefix);
    mu_run(test_parse_with_last_param);
    mu_run(test_parse_with_prefix_and_last_param);
    mu_run(test_parse_only_last_param);
    mu_run(test_parse_with_prefix_only_last_param);
}

