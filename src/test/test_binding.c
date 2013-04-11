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
#include "minunit.h"
#include "test.h"
#include "../lib/binding.c"

void target(char* txt) {
    /* target function to test hook methods */
}

void test_bnd_lookup() {
    CallbackPtr callback;

    bnd_bind("test", (CallbackPtr) target);
    callback = bnd_lookup("test");
    mu_assert(callback == (CallbackPtr) target, "test_bnd_lookup: Found a different memory address");

    bnd_unbind("test"); /* Cleanup */
    callback = bnd_lookup("test");
    mu_assert(callback == NULL, "test_bnd_lookup: callback should be NULL");
}

void test_lookup_unexisting_event() {
    CallbackPtr callback = bnd_lookup("test-unexisting");
    mu_assert(callback == NULL, "test_lookup_unexisting_event: Callback should be NULL");
}

void test_bnd_cleanup() {
    bnd_cleanup();
    mu_assert(ht == NULL, "test_bnd_cleanup: Hash table should be NULL");
}

void test_binding() {
    mu_run(test_bnd_lookup);
    mu_run(test_lookup_unexisting_event);
    mu_run(test_bnd_cleanup);

    bnd_cleanup();   /* Free memory used in tests */
}

