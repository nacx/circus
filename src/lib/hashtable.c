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
    ht_size = 256;
    ht_num_entries = 0;

    if ((ht = malloc(ht_size * sizeof(HTEntry*))) == 0) {
        perror("Out of memory (ht_init)");
        exit(EXIT_FAILURE);
    }
}

HTEntry* ht_add(HTData data) {
    HTEntry *current, *old;
    HTIndex idx;

    // Make sure hash table is always initialized
    if (!ht) {
        ht_init();
    }

    idx = ht_hash(data);
    current = ht[idx];

    // Find the key (if already exists)
    while (current && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    if (!current) { // Insert entry at beginning of list
        if ((current = malloc(sizeof(HTEntry))) == 0) {
            perror("Out of memory (ht_add)");
            exit(EXIT_FAILURE);
        }

        old = ht[idx];
        ht[idx] = current;
        current->next = old;
        current->data = data;
        ht_num_entries++;
    } else { // Replace the entry with the new one
        current->data = data;
    }

    return current;
}

void ht_del(HTData data) {
    HTEntry *current, *old;
    HTIndex idx;

    // Find entry
    old = 0;
    idx = ht_hash(data);
    current = ht[idx];

    while (current && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    if (!current)
        return;

    if (old) {
        old->next = current->next; /* not first node, old points to previous node */
    } else {
        ht[idx] = current->next; /* first node on chain */
    }

    free(current);
    ht_num_entries--;
}

HTEntry* ht_find(HTData data) {
    HTEntry *current;

    current = ht[ht_hash(data)];
    while (current && !ht_eq(current->data, data)) {
        current = current->next;
    }
    return current;
}

