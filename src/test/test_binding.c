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
    /* Target function to test hook methods */
}

void test_bnd_init() {
    bnd_init();

    mu_assert(bindings != NULL, "test_bnd_init: bindings should not be NULL");
    mu_assert(bindings->table != NULL, "test_bnd_init: binding table should not be NULL");
    mu_assert(bindings->lock != NULL, "test_bnd_init: binding lock should not be NULL");
}

void test_bnd_lookup() {
    Callback callback;

    bnd_bind("test", (Callback) target);
    callback = bnd_lookup("test");
    mu_assert(callback == (Callback) target, "test_bnd_lookup: Found a different memory address");

    bnd_unbind("test"); /* Cleanup */
    callback = bnd_lookup("test");
    mu_assert(callback == NULL, "test_bnd_lookup: callback should be NULL");
}

void test_lookup_unexisting_event() {
    Callback callback = bnd_lookup("test-unexisting");
    mu_assert(callback == NULL, "test_lookup_unexisting_event: Callback should be NULL");
}

void test_bnd_destroy() {
    bnd_destroy();
    mu_assert(bindings == NULL, "test_bnd_destroy: Binding table should be NULL");
}

void test_binding() {
    mu_run(test_bnd_init);
    mu_run(test_bnd_lookup);
    mu_run(test_lookup_unexisting_event);
    mu_run(test_bnd_destroy);
}

