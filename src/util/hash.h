#pragma once

#include <stdint.h>

typedef struct node {
    void* value;
    // the key is NULL if the node is empty
    const char* key;
} HashNode;

typedef struct {
    int stored;       // # of items in hashtable
    int capacity;     // # of entries that can fit in the table
    HashNode* entries;// collection of entries
} HashMap;

// initialize. returns pointer to the HashMap
HashMap* hm_init(int num_buckets);

// free memory
void hm_free(HashMap *hm);

// find element
void* hm_lookup(HashMap *hm, const char* key);

// add element. VERY IMPORTANTLY, the hashtable does *not* copy the string 
// you give it as a key. If you de-allocate that string, the hashtable gets 
// a bunch of undefined behavior (like seg-faulting at free)
int hm_set(HashMap *hm, const char *key, void *value);

// remove element. DOES NOT DEALLOCATE THE SPACE FOR THE REMOVED ELEMENTS
int hm_remove(HashMap *hm, const char* key);

// hashes the keys
uint64_t hash(const char* key);