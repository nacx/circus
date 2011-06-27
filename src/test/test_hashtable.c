#include <stdio.h>
#include "minunit.h"
#include "test.h"
#include "../lib/hashtable.h"

char* test_ht_init() {
	ht_init();
	mu_assert(ht != NULL, "test_ht_init: Hash table should exist");
	mu_assert(ht_num_entries == 0, "test_hook: ht_num_entries should be 0");
	return 0;
}

char* test_ht_add() {
	HTData data;
	HTIndex idx;

	data.key = "test-key";
	data.value = NULL;
	ht_add(data);

	idx = ht_hash(data);
	mu_assert(ht[idx], "test_ht_add: ht[idx] should exist");
	mu_assert(ht_num_entries == 1, "test_hook: ht_num_entries should be 1");

	// Cleanup
	ht_del(data);

	return 0;
}

char* test_ht_add_replace() {
	HTData data, data2;

	data.key = "test-key";
	data.value = NULL;
	ht_add(data);

	data2.key = "test-key";
	data2.value = NULL;
	ht_add(data2);

	mu_assert(ht_num_entries == 1, "test_ht_add_replace: ht_num_entries should be 1");

	// Cleanup
	ht_del(data2);

	return 0;
}

char* test_ht_del() {
	HTData data;
	HTIndex idx;

	data.key = "test-key";
	data.value = NULL;
	ht_add(data);

	idx = ht_hash(data);
	ht_del(data);

	mu_assert(!ht[idx], "test_ht_del: ht[idx] should not exist");
	mu_assert(ht_num_entries == 0, "test_ht_add_replace: ht_num_entries should be 0");

	return 0;
}

char* test_ht_del_unexisting() {
	HTData data;
	HTIndex idx;

	data.key = "test-key";
	data.value = NULL;

	idx = ht_hash(data);
	ht_del(data);

	mu_assert(!ht[idx], "test_ht_del_unexisting: ht[idx] should not exist");

	return 0;
}

char* test_ht_find() {
	HTData data;
	HTEntry* entry;

	data.key = "test-key";
	data.value = NULL;
	ht_add(data);

	entry = ht_find(data);
	mu_assert(entry->data.key == data.key, "test_ht_find: key does not match");

	// Cleanup
	ht_del(data);

	return 0;
}

char* test_ht_find_unexisting() {
	HTData data;
	HTEntry* entry;

	data.key = "test-key";
	data.value = NULL;

	entry = ht_find(data);
	mu_assert(!entry, "test_ht_find_unexisting: Entry should not exist");

	return 0;
}

char* test_hashtable() {
	mu_run(test_ht_init);
	mu_run(test_ht_add);
	mu_run(test_ht_add_replace);
	mu_run(test_ht_del);
	mu_run(test_ht_del_unexisting);
	mu_run(test_ht_find);
	mu_run(test_ht_find_unexisting);
	return 0;
}
