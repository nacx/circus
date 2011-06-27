#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/************************/
/* Hash table functions */
/************************/

HTIndex ht_hash(HTData data) {
	HTIndex h = 0;
	char* str = data.key;
	while (*str)
		h += *str++;
	return h;
}

void ht_init() {
	ht_size = 256;
	if ((ht = malloc(ht_size * sizeof(HTEntry*))) == 0) {
		fprintf(stderr, "Out of memory (ht_init)\n");
		exit(1);
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

	if (!current) {	// Insert entry at beginning of list
		if ((current = malloc(sizeof(HTEntry))) == 0) {
			fprintf(stderr, "Out of memory (ht_add)\n");
			exit(1);
		}

		old = ht[idx];
		ht[idx] = current;
		current->next = old;
		current->data = data;
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
}

HTEntry* ht_find(HTData data) {
	HTEntry *current;

	current = ht[ht_hash(data)];
	while (current && !ht_eq(current->data, data)) {
		current = current->next;
	}
	return current;
}
