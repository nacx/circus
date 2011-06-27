#include <stdio.h>
#include "hook.h"
#include "hashtable.h"

/******************/
/* Hook functions */
/******************/

void hook(char* key, HOOK hook) {
	HTData entry;

	entry.key = key;
	entry.value = hook;

	ht_add(entry);
}

void unhook(char* key) {
	HTData entry;

	entry.key = key;
	entry.value = NULL;

	ht_del(entry);
}

HOOK lookup(char* key) {
	HTData entry;
	HOOK hook = NULL;

	entry.key = key;
	entry.value = NULL;

	HTEntry* current = ht_find(entry);
	if (current) {
		hook = current->data.value;
	}

	return hook;
}

