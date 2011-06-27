#include <stdio.h>
#include "minunit.h"
#include "test.h"

int tests_run = 0;

char* all_tests() {
	mu_suite(test_hashtable);
	mu_suite(test_hook);
	return 0;
}

int main(int argc, char **argv) {
	printf("----------------------------------------\n");
	printf("Running unit tests...\n");

	char* result = all_tests();

	printf("  Tests run: %d\n", tests_run);

	if (result == 0) {
		printf("  Test result: Ok\n");
	} else {
		printf("  Test result: Failure (%s)\n", result);
	}

	printf("----------------------------------------\n");

	return result != 0;
}
