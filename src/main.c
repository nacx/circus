#include <stdio.h>
#include "lib/hook.h"

void test_hook(char* text) {
	printf("%s\n", text);
}

int main(int argc, char **argv) {

	hook("TEST", test_hook);
	HOOK h = lookup("TEST");

	if (h) {
		h("This is the hook argument");
	} else {
		printf("No hook found!\n");
	}

	unhook("TEST");
	h = lookup("TEST");

	if (h) {
		h("This is the hook argument");
	} else {
		printf("No hook found!\n");
	}

	return 0;
}
