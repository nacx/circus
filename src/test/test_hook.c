#include <stdio.h>
#include "minunit.h"
#include "test.h"
#include "../lib/hashtable.h"
#include "../lib/hook.h"

void target(char* txt) {
	// target function to test hook methods
}

char* test_hook_method() {
	hook("test", target);
	mu_assert(ht_num_entries == 1, "test_hook: ht_num_entries should be 1");
	return 0;
}

char* test_unhook() {
	hook("test", target);
	unhook("test");
	mu_assert(ht_num_entries == 0, "test_unhook: ht_num_entries should be 0");
	return 0;
}

char* test_lookup() {
	hook("test", target);
	HOOK h = lookup("test");
	mu_assert(h == target, "test_lookup: Found a different memory address");
	return 0;
}

char* test_hook() {
	mu_run(test_hook_method);
	mu_run(test_unhook);
	mu_run(test_lookup);
	return 0;
}
