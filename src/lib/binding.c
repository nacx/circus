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
#include "binding.h"
#include "hashtable.h"


void bind_event(char* event, void* callback) {
    HTData entry;

    entry.key = event;
    entry.value = callback;

    ht_add(entry, 1);   // Add the key and override it
}

char* unbind_event(char* event) {
    HTData entry;

    entry.key = event;
    entry.value = NULL;

    entry = ht_del(entry);
    return entry.key;
}

void* lookup_event(char* event) {
    HTData entry;
    void* callback = NULL;

    entry.key = event;
    entry.value = NULL;

    HTEntry* current = ht_find(entry);
    if (current != NULL) {
        callback = current->data.value;
    }

    return callback;
}

