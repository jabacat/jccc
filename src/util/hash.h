#pragma once

#include <stdint.h>

typedef struct node {
    void* value;
    // the key is NULL if the node is empty
    char* key;
} hashNode;

typedef struct {
    int stored;       // # of items in hashtable
    int capacity;     // # of entries that can fit in the table
    hashNode* entries;// collection of entries
} hashMap;

// initialize. returns pointer to the hashMap
hashMap* hm_init(int num_buckets);

// free memory
void hm_free(hashMap *hm);

// find element
void* hm_lookup(hashMap *hm, const char* key);

// add element
int hm_set(hashMap *hm, const char *key, void *value);

// remove element (unimplemented)
void hm_remove(hashMap *hm, const char* key);

// hashes the keys
uint64_t hash(const char* key);