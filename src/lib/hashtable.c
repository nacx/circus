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
#include "utils.h"
#include "hashtable.h"


/* ****************** */
/* Hast table helpers */
/* ****************** */

/* Compute the hash of the given key */
static unsigned char ht_hash(char* key) {
    unsigned char hash = 0;
    char* str = key;
    while (*str) {
        hash += *str++;
    }
    return hash;
}

/* Create the structure that will hold a key and its values */
static struct ht_data* ht_data_create(char* key, void* value, Function function) {
    struct ht_data* data;

    if ((data = malloc(sizeof(struct ht_data))) == 0) {
        perror("Out of memory (ht_data_create)");
        exit(EXIT_FAILURE);
    }

    /* Allocate length + 1 to make space for the '\0' character */
    if ((data->key = malloc((strlen(key) + 1) * sizeof(char))) == 0) {
        perror("Out of memory (ht_data_create: key)");
        exit(EXIT_FAILURE);
    }

    strncpy(data->key, key, strlen(key) + 1);
    data->value = value;
    data->function = function;

    return data;
}

/* Destruy a ht_data structure */
static void ht_data_destroy(struct ht_data* data) {
    if (data != NULL) {
        free(data->key);
        free(data);
    }
}

/* ************************* */
/* Hash table implementation */
/* ************************* */

struct ht_table* ht_create() {
    int i;
    struct ht_table* ht;

    debug(("hashtable: Creating table of size %d\n", HT_SIZE));

    if ((ht = malloc(sizeof(struct ht_table))) == 0) {
        perror("Out of memory (ht_create)");
        exit(EXIT_FAILURE);
    }

    if ((ht->entries = malloc(HT_SIZE * sizeof(struct ht_entry*))) == 0) {
        perror("Out of memory (ht_create: entries)");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < HT_SIZE; i++) {
        ht->entries[i] = NULL;
    }

    ht->num_entries = 0;
    ht->size = HT_SIZE;

    return ht;
}

void ht_destroy(struct ht_table* ht) {
    struct ht_entry* current, *previous;
    int i = 0;

    debug(("hashtable: Destroying\n"));

    for (i = 0; i < ht->size; i++) {
        if (ht->entries[i] != NULL) {
            current = ht->entries[i];
            while (current != NULL) {
                previous = current;
                current = current->next;
                ht_data_destroy(previous->data);
                free(previous);
            }
        }
    }

    free(ht->entries);
    free(ht);
}

static void ht_add(struct ht_table* ht, char* key, void* value, void(*function)(void)) {
    struct ht_entry *current, *old;
    unsigned char idx;
    struct ht_data* data;

    debug(("hashtable: Adding entry with key %s\n", key));

    idx = ht_hash(key);
    current = ht->entries[idx];

    /* Find the key (if already exists) */
    while (current != NULL && s_ne(current->data->key, key)) {
        old = current;
        current = current->next;
    }

    /* If key does not exist or it must not be overriden,
     * add the new element at the beginning of the list. */
    data = ht_data_create(key, value, function);

    if (current == NULL) {  /* Add element */
        if ((current = malloc(sizeof(struct ht_entry))) == 0) {
            perror("Out of memory (ht_add)");
            exit(EXIT_FAILURE);
        }

        old = ht->entries[idx];
        ht->entries[idx] = current;
        current->next = old;
        current->data = data;
        ht->num_entries++;
    } else {    /* Overwrite */
        ht_data_destroy(current->data);
        current->data = data;
    }
}

void ht_add_value(struct ht_table* ht, char* key, void* value) {
    ht_add(ht, key, value, NULL);
}

void ht_add_function(struct ht_table* ht, char* key, void(*function)(void)) {
    ht_add(ht, key, NULL, function);
}

void ht_del(struct ht_table* ht, char* key) {
    struct ht_entry *current, *old;
    unsigned char idx;

    debug(("hashtable: Deleting entry with key %s\n", key));

    /* Find entry */
    old = 0;
    idx = ht_hash(key);
    current = ht->entries[idx];

    while (current != NULL && s_ne(current->data->key, key)) {
        old = current;
        current = current->next;
    }

    if (current != NULL) {
        if (old != NULL) {
            old->next = current->next;          /* Not first node, old points to previous node */
        } else {
            ht->entries[idx] = current->next;   /* First node on chain */
        }

        ht_data_destroy(current->data);
        free(current);
        ht->num_entries--;
    }
}

struct ht_data* ht_find(struct ht_table* ht, char* key) {
    struct ht_entry *current;
    
    debug(("hashtable: Looking for key %s\n", key));

    current = ht->entries[ht_hash(key)];
    while (current != NULL && s_ne(current->data->key, key)) {
        current = current->next;
    }

    debug(("hashtable: Key %s %sfound\n", key, current == NULL? "not " : ""));

    return current == NULL? NULL : current->data;
}

void ht_print_keys(struct ht_table* ht) {
    struct ht_entry* current;
    int i = 0;

    debug(("hashtable: Dumping keys\n"));

    for (i = 0; i < ht->size; i++) {
        if (ht->entries[i] != NULL) {
            current = ht->entries[i];
            while (current != NULL) {
                printf("Key: %s\n", current->data->key);
                current = current->next;
            }
        }
    }
}

