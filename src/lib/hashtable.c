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
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"


HTIndex ht_hash(HTData data) {
    HTIndex h = 0;
    char* str = data.key;
    while (*str)
        h += *str++;
    return h;
}

void ht_init() {
    int i;
    ht_size = 256;
    ht_num_entries = 0;

    if ((ht = malloc(ht_size * sizeof(HTEntry*))) == 0) {
        perror("Out of memory (ht_init)");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < ht_size; i++) {
        ht[i] = NULL;
    }
}

HTEntry* ht_add(HTData data, int override) {
    HTEntry *current, *old;
    HTIndex idx;

    // Make sure hash table is always initialized
    if (ht == NULL) {
        ht_init();
    }

    idx = ht_hash(data);
    current = ht[idx];

    // Find the key (if already exists)
    while (current != NULL && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    // If key does not exist or it must not be overriden,
    // add the new element at the beginning of the list
    if (current == NULL || override == 0) {
        if ((current = malloc(sizeof(HTEntry))) == 0) {
            perror("Out of memory (ht_add)");
            exit(EXIT_FAILURE);
        }

        old = ht[idx];
        ht[idx] = current;
        current->next = old;
        current->data = data;
        ht_num_entries++;
    } else {
        current->data = data;
    }

    return current;
}

HTData ht_del(HTData data) {
    HTEntry *current, *old;
    HTIndex idx;
    HTData ret;

    ret.key = NULL;
    ret.value = NULL;

    // Find entry
    old = 0;
    idx = ht_hash(data);
    current = ht[idx];

    while (current != NULL && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    if (current == NULL)
        return ret;

    if (old != NULL) {
        old->next = current->next; /* not first node, old points to previous node */
    } else {
        ht[idx] = current->next; /* first node on chain */
    }

    ret = current->data;
    free(current);
    ht_num_entries--;

    return ret;
}

HTEntry* ht_find(HTData data) {
    HTEntry *current;

    current = ht[ht_hash(data)];
    while (current != NULL && !ht_eq(current->data, data)) {
        current = current->next;
    }
    return current;
}

void ht_print_keys() {
    HTEntry* current;
    int i = 0;

    for (i = 0; i < ht_size; i++) {
        if (ht[i] != NULL) {
            current = ht[i];
            while (current != NULL) {
                printf("Key: %s\n", current->data.key);
                current = current->next;
            }
        }
    }
}

