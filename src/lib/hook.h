#ifndef __HOOK_H__
#define __HOOK_H__

// The hook pointer type
typedef void (*HOOK)(char*);

// Hook functions
void 	hook(char* key, HOOK hook);		// Hook a function to a given key
void 	unhook(char* key);				// Remove the hook for the given key
HOOK 	lookup(char* key);				// Lookup the given hook

#endif
