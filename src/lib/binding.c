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

#define _POSIX_SOURCE   /* Use POSIX threads */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "debug.h"
#include "hashtable.h"
#include "binding.h"


/* The thread safe binding table data type */
struct bnd_table {
    struct ht_table* table;      /* The binding table */
    pthread_mutex_t* lock;              /* Prevent race conditions if user callbacks modify the bindings */
};

/* The binding table */
static struct bnd_table* bindings;


static void bnd_init() {
    debug(("binding: Creating table\n"));

    if ((bindings = malloc(sizeof(struct bnd_table))) == 0) {
        perror("Out of memory (bnd_init: table)");
        exit(EXIT_FAILURE);
    }

    bindings->table = ht_create();

    debug(("binding: Creating binding lock\n"));
    if ((bindings->lock = malloc(sizeof(pthread_mutex_t))) == 0) {
        perror("Out of memory (bnd_init: lock)");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(bindings->lock, NULL);
}

void bnd_bind(char* event, CallbackPtr callback) {
    debug(("binding: Adding event %s\n", event));
    if (bindings == NULL) {
        bnd_init();
    }
    pthread_mutex_lock(bindings->lock);
    ht_add_function(bindings->table, event, callback);
    pthread_mutex_unlock(bindings->lock);
}

void bnd_unbind(char* event) {
    debug(("binding: Removing event %s\n", event));
    pthread_mutex_lock(bindings->lock);
    ht_del(bindings->table, event);
    pthread_mutex_unlock(bindings->lock);
}

CallbackPtr bnd_lookup(char* event) {
    struct ht_data* data;
    debug(("binding: Looking for event %s\n", event));
    data = ht_find(bindings->table, event);
    return data == NULL? NULL : data->function;
}

void bnd_destroy() {
    if (bindings != NULL) {
        debug(("binding: Cleaning up binding table\n"));
        ht_destroy(bindings->table);

        debug(("binding: Destroying binding lock\n"));
        pthread_mutex_destroy(bindings->lock);
        free(bindings->lock);

        free(bindings);
        bindings = NULL;
    }
}

