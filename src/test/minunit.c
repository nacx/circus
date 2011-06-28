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

#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

struct failure {
	struct failure *next;
	char* msg;
};

struct failure *fails;

int tests_run = 0;
int test_fails = 0;


void mu_fail(char* msg) {
	struct failure *f, *it;

	if ((f = malloc(sizeof(struct failure))) == 0) {
		perror("Out of memory (fail)");
		exit(1);
	}

	f->msg = msg;
	f->next = NULL;

	// Add the error at the end of the list
	for (it = fails; it && it->next; it = it->next);

	if (!it) { // First error
		fails = f;
	} else {
		it->next = f;
	}

	test_fails++;
}

void mu_results() {
	struct failure *f;

	printf("  Tests run: %d\n", tests_run);

	if (test_fails == 0) {
		printf("  Test result: Ok\n");
	} else {
		printf("  Test result: %d Failures\n", test_fails);

		for (f = fails; f; f = f->next) {
			printf("    %s\n", f->msg);
		}
	}
}

void mu_free() {
	struct failure *old, *current;

	old = current = fails;

	while (current) {
		old = current;
		current = current->next;
		free(old);
	}
}
