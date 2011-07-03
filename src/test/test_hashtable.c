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
#include "../lib/hashtable.h"

char* test_ht_init() {
    ht_init();
    mu_assert(ht != NULL, "test_ht_init: Hash table should exist");
    mu_assert(ht_num_entries == 0, "test_ht_init: ht_num_entries should be 0");
    return 0;
}

char* test_ht_add() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;
    ht_add(data, 1);

    idx = ht_hash(data);
    mu_assert(ht[idx], "test_ht_add: ht[idx] should exist");
    mu_assert(ht_num_entries == 1, "test_ht_add: ht_num_entries should be 1");

    // Cleanup
    ht_del(data);

    return 0;
}

char* test_ht_add_replace() {
    HTData data, data2;

    data.key = "test-key";
    data.value = NULL;
    ht_add(data, 1);

    data2.key = "test-key";
    data2.value = NULL;
    ht_add(data2, 1);

    mu_assert(ht_num_entries == 1, "test_ht_add_replace: ht_num_entries should be 1");

    // Cleanup
    ht_del(data2);

    return 0;
}

char* test_ht_add_append() {
    HTData data, data2;

    data.key = "test-key";
    data.value = NULL;
    ht_add(data, 0);

    data2.key = "test-key";
    data2.value = NULL;
    ht_add(data2, 0);

    mu_assert(ht_num_entries == 2, "test_ht_add_append: ht_num_entries should be 2");

    // Cleanup
    ht_del(data);
    ht_del(data2);

    return 0;
}

char* test_ht_del() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;
    ht_add(data, 1);

    idx = ht_hash(data);
    ht_del(data);

    mu_assert(!ht[idx], "test_ht_del: ht[idx] should not exist");
    mu_assert(ht_num_entries == 0, "test_ht_del: ht_num_entries should be 0");

    return 0;
}

char* test_ht_del_unexisting() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;

    idx = ht_hash(data);
    ht_del(data);

    mu_assert(!ht[idx], "test_ht_del_unexisting: ht[idx] should not exist");

    return 0;
}

char* test_ht_find() {
    HTData data;
    HTEntry* entry;

    data.key = "test-key";
    data.value = NULL;
    ht_add(data, 1);

    entry = ht_find(data);
    mu_assert(entry->data.key == data.key, "test_ht_find: key does not match");

    // Cleanup
    ht_del(data);

    return 0;
}

char* test_ht_find_unexisting() {
    HTData data;
    HTEntry* entry;

    data.key = "test-key";
    data.value = NULL;

    entry = ht_find(data);
    mu_assert(!entry, "test_ht_find_unexisting: Entry should not exist");

    return 0;
}

void test_hashtable() {
    mu_run(test_ht_init);
    mu_run(test_ht_add);
    mu_run(test_ht_add_replace);
    mu_run(test_ht_add_append);
    mu_run(test_ht_del);
    mu_run(test_ht_del_unexisting);
    mu_run(test_ht_find);
    mu_run(test_ht_find_unexisting);
}

