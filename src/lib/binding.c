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
#include <stdio.h>
#include "debug.h"
#include "hashtable.h"
#include "binding.h"

static HTable* ht = NULL;

void bind_event(char* event, CallbackPtr callback) {
    HTData data;

    if (ht == NULL) {
        debug(("binding: Creating table\n"));
        ht = ht_create();
    }

    data.key = event;
    data.function = callback;

    debug(("binding: Adding event %s\n", event));
    ht_add(ht, data);
}

char* unbind_event(char* event) {
    HTData data;

    data.key = event;
    data.value = NULL;
    data.function = NULL;

    debug(("binding: Removing event %s\n", event));
    data = ht_del(ht, data);
    return data.key;
}

CallbackPtr lookup_event(char* event) {
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

void cleanup_bindings() {
    if (ht != NULL) {
        debug(("binding: Cleaning up\n"));
        ht_destroy(ht);
        ht = NULL;
    }
}

