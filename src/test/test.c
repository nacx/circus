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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "minunit.h"
#include "test.h"


void run_all_tests() {
    mu_suite(test_version);
    mu_suite(test_hashtable);
    mu_suite(test_binding);
    mu_suite(test_utils);
    mu_suite(test_codes);
    mu_suite(test_events);
    mu_suite(test_network);
    mu_suite(test_listener);
    mu_suite(test_irc);
    mu_suite(test_dispatcher);
}

int disable_stdout() {
    int original, quiet;
    fflush(stdout);
    original = dup(1);
    quiet = open("/dev/null", O_WRONLY);
    dup2(quiet, 1);
    close(quiet);
    return original;
}

void enable_stdout(int original) {
    fflush(stdout);
    dup2(original, 1);
    close(original);
}

int main(int argc, char **argv) {
    int _stdout;
    printf("--------------------------------------------------\n");
    printf("Running unit tests...\n");

    _stdout = disable_stdout();
    run_all_tests();
    enable_stdout(_stdout);

    mu_results();
    mu_free();

    printf("--------------------------------------------------\n");

    return test_fails != 0;
}

