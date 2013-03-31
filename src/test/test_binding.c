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
    // target function to test hook methods
}

void test_lookup_event() {
    bind_event("test", target);
    void* callback = lookup_event("test");
    mu_assert(callback == target, "test_lookup_event: Found a different memory address");
    unbind_event("test"); // Cleanup
}

void test_lookup_unexisting_event() {
    void* callback = lookup_event("test-unexisting");
    mu_assert(callback == NULL, "test_lookup_unexisting_event: Callback should be NULL");
}

void test_cleanup_bindings() {
    cleanup_bindings();
    mu_assert(ht == NULL, "test_cleanup_bindings: Hash table should be NULL");
}

void test_binding() {
    mu_run(test_lookup_event);
    mu_run(test_lookup_unexisting_event);
    mu_run(test_cleanup_bindings);
}

