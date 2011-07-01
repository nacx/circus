/*
 * Copyright (c) 2010 Ignasi Barrera
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

#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "test.h"
#include "../lib/message_handler.h"


char* test_parse_empty_message() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(NULL, buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    raw = parse("", buffer);

    mu_assert(raw.prefix == NULL, "test_parse_empty_message: prefix should be NULL"); 
    mu_assert(raw.type == NULL, "test_parse_empty_message: type should be NULL"); 
    mu_assert(raw.num_params == 0, "test_parse_empty_message: there should be 0 parameters");

    return 0;
}

char* test_parse() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse("TEST This is a message test without prefix", buffer);

    mu_assert(raw.prefix == NULL, "test_parse: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse: type should be TEST"); 
    mu_assert(raw.num_params == 7, "test_parse: there should be 7 parameters");

    return 0;
}

char* test_parse_with_prefix() {
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":prefix TEST This is a message test with prefix", buffer);

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix: type should be 'TEST'"); 
    mu_assert(raw.num_params == 7, "test_parse_with_prefix: there should be 7 parameters");

    return 0;
}

char* test_parse_with_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse("TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params -1];

    mu_assert(raw.prefix == NULL, "test_parse_with_last_param: prefix should be NULL"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_last_param: last parameter should be 'last parameter'");

    return 0;
}

char* test_parse_with_prefix_and_last_param() {
    char* last_param;
    struct raw_msg raw;
    char* buffer = NULL;

    raw = parse(":prefix TEST This is a message test with a :last parameter", buffer);
    last_param = raw.params[raw.num_params - 1];

    mu_assert(s_eq(raw.prefix, "prefix"), "test_parse_with_prefix_and_last_param: prefix should be 'prefix'"); 
    mu_assert(s_eq(raw.type, "TEST"), "test_parse_with_prefix_and_last_param: type should be 'TEST'"); 
    mu_assert(raw.num_params == 8, "test_parse_with_prefix_and_last_param: there should be 8 parameters");
    mu_assert(s_eq(last_param, "last parameter"), "test_parse_with_prefix_and_last_param: last parameter should be 'last parameter'");

    return 0;
}

void test_message_handler() {
    mu_run(test_parse_empty_message);
    mu_run(test_parse);
    mu_run(test_parse_with_prefix);
    mu_run(test_parse_with_last_param);
    mu_run(test_parse_with_prefix_and_last_param);
}

