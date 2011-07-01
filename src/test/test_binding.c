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
#include "minunit.h"
#include "test.h"
#include "../lib/hashtable.h"
#include "../lib/binding.h"

void target(char* txt) {
    // target function to test hook methods
}

char* test_bind_event() {
    bind_event("test", target);
    mu_assert(ht_num_entries == 1, "test_bind_event: ht_num_entries should be 1");
    unbind_event("test"); // Cleanup
    return 0;
}

char* test_unbind_event() {
    bind_event("test", target);
    unbind_event("test");
    mu_assert(ht_num_entries == 0, "test_unbind_event: ht_num_entries should be 0");
    return 0;
}

char* test_lookup_event() {
    bind_event("test", target);
    void* callback = lookup_event("test");
    mu_assert(callback == target, "test_lookup_event: Found a different memory address");
    unbind_event("test"); // Cleanup
    return 0;
}

char* test_lookup_unexisting_event() {
    void* callback = lookup_event("test");
    mu_assert(callback == NULL, "test_lookup_unexisting_event: Callback should be NULL");
    return 0;
}

void test_binding() {
    mu_run(test_bind_event);
    mu_run(test_unbind_event);
    mu_run(test_lookup_event);
    mu_run(test_lookup_unexisting_event);
}

