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

static HTable* ht = NULL;           /* The binding table */
static pthread_mutex_t* bnd_lock;   /* Prevent race conditions if user callbacks modify the bindings */


static void bnd_init() {
    debug(("binding: Creating table\n"));
    ht = ht_create();

    debug(("binding: Creating binding lock\n"));
    if ((bnd_lock = malloc(sizeof(pthread_mutex_t))) == 0) {
        perror("Out of memory (q_create: lock)");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(bnd_lock, NULL);
}

void bnd_bind(char* event, CallbackPtr callback) {
    HTData data;

    if (ht == NULL) {
        bnd_init();
    }

    data.key = event;
    data.function = callback;

    debug(("binding: Adding event %s\n", event));

    pthread_mutex_lock(bnd_lock);
    ht_add(ht, data);
    pthread_mutex_unlock(bnd_lock);
}

char* bnd_unbind(char* event) {
    HTData data;

    data.key = event;
    data.value = NULL;
    data.function = NULL;

    debug(("binding: Removing event %s\n", event));

    pthread_mutex_lock(bnd_lock);
    data = ht_del(ht, data);
    pthread_mutex_unlock(bnd_lock);

    return data.key;
}

CallbackPtr bnd_lookup(char* event) {
    HTData data;
    HTEntry* entry;
    CallbackPtr callback = NULL;

    debug(("binding: Looking for event %s\n", event));

    data.key = event;
    data.value = NULL;
    data.function = NULL;

    entry = ht_find(ht, data);
    if (entry != NULL) {
        callback = entry->data.function;
    }

    return callback;
}

void bnd_cleanup() {
    if (ht != NULL) {
        debug(("binding: Cleaning up binding table\n"));
        ht_destroy(ht);
        ht = NULL;

        debug(("binding: Destroying binding lock\n"));
        pthread_mutex_destroy(bnd_lock);
        free(bnd_lock);
    }
}

