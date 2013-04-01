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
#include "debug.h"
#include "hashtable.h"


HTIndex ht_hash(HTData data) {
    HTIndex h = 0;
    char* str = data.key;
    while (*str)
        h += *str++;
    return h;
}

HTable* ht_create() {
    int i;
    HTable* ht;
    HTEntry** entries;

    debug(("hashtable: Creating table of size %d\n", HT_SIZE));

    if ((entries = malloc(HT_SIZE * sizeof(HTEntry*))) == 0) {
        perror("Out of memory (ht_create_entries)");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < HT_SIZE; i++) {
        entries[i] = NULL;
    }

    if ((ht = malloc(sizeof(HTable))) == 0) {
        perror("Out of memory (ht_create)");
        exit(EXIT_FAILURE);
    }

    ht->num_entries = 0;
    ht->size = HT_SIZE;
    ht->entries = entries;

    return ht;
}

void ht_destroy(HTable* ht) {
    HTEntry* current, *previous;
    int i = 0;

    debug(("hashtable: Destroying\n"));

    for (i = 0; i < ht->size; i++) {
        if (ht->entries[i] != NULL) {
            current = ht->entries[i];
            while (current != NULL) {
                previous = current;
                current = current->next;
                free(previous);
            }
        }
    }

    free(ht->entries);
    free(ht);
}

HTEntry* ht_add(HTable* ht, HTData data) {
    HTEntry *current, *old;
    HTIndex idx;

    debug(("hashtable: Adding entry with key %s\n", data.key));

    idx = ht_hash(data);
    current = ht->entries[idx];

    /* Find the key (if already exists) */
    while (current != NULL && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    /* If key does not exist or it must not be overriden, */
    /* add the new element at the beginning of the list */
    if (current == NULL) {
        if ((current = malloc(sizeof(HTEntry))) == 0) {
            perror("Out of memory (ht_add)");
            exit(EXIT_FAILURE);
        }

        old = ht->entries[idx];
        ht->entries[idx] = current;
        current->next = old;
        current->data = data;
        ht->num_entries++;
    } else {
        current->data = data;
    }

    return current;
}

HTData ht_del(HTable* ht, HTData data) {
    HTEntry *current, *old;
    HTIndex idx;
    HTData ret;
    
    debug(("hashtable: Deleting entry with key %s\n", data.key));

    ret.key = NULL;
    ret.value = NULL;
    ret.function = NULL;

    /* Find entry */
    old = 0;
    idx = ht_hash(data);
    current = ht->entries[idx];

    while (current != NULL && !ht_eq(current->data, data)) {
        old = current;
        current = current->next;
    }

    if (current == NULL)
        return ret;

    if (old != NULL) {
        old->next = current->next;          /* Not first node, old points to previous node */
    } else {
        ht->entries[idx] = current->next;   /* First node on chain */
    }

    ret = current->data;
    free(current);
    ht->num_entries--;

    return ret;
}

HTEntry* ht_find(HTable* ht, HTData data) {
    HTEntry *current;
    
    debug(("hashtable: Looking for key %s\n", data.key));

    current = ht->entries[ht_hash(data)];
    while (current != NULL && !ht_eq(current->data, data)) {
        current = current->next;
    }
    return current;
}

void ht_print_keys(HTable* ht) {
    HTEntry* current;
    int i = 0;

    debug(("hashtable: Dumping keys\n"));

    for (i = 0; i < ht->size; i++) {
        if (ht->entries[i] != NULL) {
            current = ht->entries[i];
            while (current != NULL) {
                printf("Key: %s\n", current->data.key);
                current = current->next;
            }
        }
    }
}

