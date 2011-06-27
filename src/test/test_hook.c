/*
 * Copyright (c) 2010 Ignasi Barrera
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
