#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

/*************************/
/* Hash table definition */
/*************************/

// The data being stored in the hashtable
typedef struct {
	char* key;
	void* value;
} HTData;

// Data equallity macro
#define ht_eq(a, b) (strcmp(a.key, b.key) == 0)

// Hash table entry
typedef struct entry {
    struct entry *next;
    HTData data;
} HTEntry;

typedef unsigned char 	HTIndex;	// The hash table index type
HTEntry**				ht;			// The hash table
int						ht_size;	// The hash table size


/************************/
/* Hash table functions */
/************************/

void 		ht_init();					// Initialize the hash table
HTIndex 	ht_hash(HTData data);		// Compute the hash of the given data
HTEntry* 	ht_add(HTData data);		// Add an entry to the hash table
void 		ht_del(HTData data);		// Remove an entry from the hash table
HTEntry* 	ht_find(HTData data);		// Find an entry in the hash table

#endif
