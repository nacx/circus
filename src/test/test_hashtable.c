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
#include "../lib/hashtable.c"

static struct ht_table* ht = NULL;

void test_ht_create() {
    ht = ht_create();
    mu_assert(ht != NULL, "test_ht_create: Hash table should exist");
    mu_assert(ht->num_entries == 0, "test_ht_create: ht->num_entries should be 0");
}

void test_ht_add_value() {
    unsigned char idx;
    char* key = "test-key";

    ht_add_value(ht, key, key);
    idx = ht_hash(key);

    mu_assert(ht->entries[idx] != NULL, "test_ht_add: ht->entries[idx] should not be NULL");
    mu_assert(ht->num_entries == 1, "test_ht_add: ht->num_entries should be 1");

    ht_del(ht, key);
}

void test_ht_add_replace() {
    char* key = "test-key";

    ht_add_value(ht, key, key);
    ht_add_value(ht, key, key);

    mu_assert(ht->num_entries == 1, "test_ht_add_replace: ht->num_entries should be 1");

    ht_del(ht, key);
}

void test_ht_add_function() {
    unsigned char idx;
    char* key = "test-key";

    ht_add_function(ht, key, test_ht_add_function);
    idx = ht_hash(key);

    mu_assert(ht->entries[idx] != NULL, "test_ht_add: ht->entries[idx] should not be NULL");
    mu_assert(ht->num_entries == 1, "test_ht_add: ht->num_entries should be 1");

    ht_del(ht, key);
}

void test_ht_del() {
    unsigned char idx;
    char* key = "test-key";

    ht_add_value(ht, key, key);

    idx = ht_hash(key);
    ht_del(ht, key);

    mu_assert(ht->entries[idx] == NULL, "test_ht_del: ht->entries[idx] should be NULL");
    mu_assert(ht->num_entries == 0, "test_ht_del: ht->num_entries should be 0");
}

void test_ht_del_unexisting() {
    char* key = "test-key";
    unsigned char idx;

    idx = ht_hash(key);
    ht_del(ht, key);

    mu_assert(ht->entries[idx] == NULL, "test_ht_del_unexisting: ht->entries[idx] should be NULL");
}

void test_ht_find() {
    char* key = "test-key";
    struct ht_data* data;

    ht_add_value(ht, key, key);

    data = ht_find(ht, key);
    mu_assert(data != NULL, "test_ht_find: Entry should not be NULL");
    mu_assert(s_eq(data->key, key), "test_ht_find: key does not match");

    ht_del(ht, key);
}

void test_ht_find_unexisting() {
    char* key = "test-key";
    struct ht_data* data;

    data = ht_find(ht, key);
    mu_assert(data == NULL, "test_ht_find_unexisting: Entry should be NULL");
}

void test_ht_destroy() {
    ht_destroy(ht);
}

void test_hashtable() {
    mu_run(test_ht_create);
    mu_run(test_ht_add_value);
    mu_run(test_ht_add_replace);
    mu_run(test_ht_del);
    mu_run(test_ht_del_unexisting);
    mu_run(test_ht_find);
    mu_run(test_ht_find_unexisting);
    mu_run(test_ht_destroy);

}

