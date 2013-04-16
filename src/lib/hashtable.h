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

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#define HT_SIZE 256             /* Number of entries in the hash table */


/*************************/
/* Hash table definition */
/*************************/

/* The data being stored in the hashtable */
struct ht_data {
    char* key;                  /* The key for the data */
    void* value;                /* The object value of the data, if value is an object or pointer */
    void (*function)(void);     /* The function pointer of the data, if value is a function pointer */
};

/* An entry in the hash table */
struct ht_entry {
    struct ht_entry *next;      /* Pointer for the next entry of the same hash */
    struct ht_data* data;       /* Data stored in the current entry */
};

/* Hash table data structure */
struct ht_table {
    struct ht_entry** entries;	    /* The hash table */
    int size;                       /* The size of the hash table */
    int num_entries;                /* The number of current entries */
};

/************************/
/* Hash table functions */
/************************/

struct ht_table*    ht_create();			        /* Creates a new hash table */
void                ht_destroy(struct ht_table* ht);            /* Destroy the given hash table */
void                ht_add_value(struct ht_table* ht, char* key, void* value);                  /* Add a value to the hash table */
void                ht_add_function(struct ht_table* ht, char* key, void(*function)(void));    /* Add a function to the hash table */
void                ht_del(struct ht_table* ht, char* key);     /* Remove an entry from the hash table */
struct ht_data*     ht_find(struct ht_table* ht, char* key);	/* Find an entry in the hash table */
void                ht_print_keys(struct ht_table* ht);         /* Print all keys in the table */

#endif

