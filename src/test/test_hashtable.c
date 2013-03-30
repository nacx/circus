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

static HTable* ht = NULL;

void test_ht_create() {
    ht = ht_create();
    mu_assert(ht != NULL, "test_ht_create: Hash table should exist");
    mu_assert(ht->num_entries == 0, "test_ht_create: ht->num_entries should be 0");
}

void test_ht_add() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;
    ht_add(ht, data);

    idx = ht_hash(data);
    mu_assert(ht->entries[idx], "test_ht_add: ht->entries[idx] should exist");
    mu_assert(ht->num_entries == 1, "test_ht_add: ht->num_entries should be 1");

    // Cleanup
    ht_del(ht, data);
}

void test_ht_add_replace() {
    HTData data, data2;

    data.key = "test-key";
    data.value = NULL;
    ht_add(ht, data);

    data2.key = "test-key";
    data2.value = NULL;
    ht_add(ht, data2);

    mu_assert(ht->num_entries == 1, "test_ht_add_replace: ht->num_entries should be 1");

    // Cleanup
    ht_del(ht, data2);
}

void test_ht_del() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;
    ht_add(ht, data);

    idx = ht_hash(data);
    ht_del(ht, data);

    mu_assert(!ht->entries[idx], "test_ht_del: ht->entries[idx] should not exist");
    mu_assert(ht->num_entries == 0, "test_ht_del: ht->num_entries should be 0");
}

void test_ht_del_unexisting() {
    HTData data;
    HTIndex idx;

    data.key = "test-key";
    data.value = NULL;

    idx = ht_hash(data);
    ht_del(ht, data);

    mu_assert(!ht->entries[idx], "test_ht_del_unexisting: ht->entries[idx] should not exist");
}

void test_ht_find() {
    HTData data;
    HTEntry* entry;

    data.key = "test-key";
    data.value = NULL;
    ht_add(ht, data);

    entry = ht_find(ht, data);
    mu_assert(entry->data.key == data.key, "test_ht_find: key does not match");

    // Cleanup
    ht_del(ht, data);
}

void test_ht_find_unexisting() {
    HTData data;
    HTEntry* entry;

    data.key = "test-key";
    data.value = NULL;

    entry = ht_find(ht, data);
    mu_assert(!entry, "test_ht_find_unexisting: Entry should not exist");
}

void test_ht_destroy() {
    ht_destroy(ht);
}

void test_hashtable() {
    mu_run(test_ht_create);
    mu_run(test_ht_add);
    mu_run(test_ht_add_replace);
    mu_run(test_ht_del);
    mu_run(test_ht_del_unexisting);
    mu_run(test_ht_find);
    mu_run(test_ht_find_unexisting);
    mu_run(test_ht_destroy);

}

