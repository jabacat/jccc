#include <stdlib.h>
#include <string.h>
#include "out.h"
#include "hash.h"

const uint64_t OFFSET = 14695981039346656037;
const uint64_t FNV_PRIME = 1099511628211;

hashMap* hm_init(int num_buckets) {
    hashMap* hm = malloc(sizeof(hashMap));
    
    // bounce if out of memory
    if (hm == NULL) {
        PRINT_WARNING("out of memory");
        return NULL;
    }

    hm->stored = 0;
    hm->capacity = num_buckets;

    hm->entries = calloc(hm->capacity, sizeof(hashNode));

    // bounce if out of memory
    if (hm->entries == NULL) {
        PRINT_WARNING("out of memory, could not allocate space for %d entries", hm->capacity);
        free(hm->entries);
        return NULL;
    }

    return hm;
}

void hm_free(hashMap *hm) {
    for (int i = 0; i<hm->capacity; i++) {
        // free all the chars allocated as keys
        free(hm->entries[i].key);
    }
    free(hm->entries);
    free(hm);
}

// follows FNV-1a hashing 
uint64_t hash(const char* key) {
    uint64_t hash = OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t)(char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* hm_lookup(hashMap *hm, const char* key) {
    uint64_t ind = hash(key) % hm->capacity;
    
    for (int i = 0; i < hm->capacity-1; i++) {
        if (strcmp(key, hm->entries[ind].key) == 0) {
            return hm->entries[ind].value;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return NULL;
}

int hm_set(hashMap *hm, const char *key, void *value) {
    uint64_t ind = hash(key) % hm->capacity;
    
    for (int i = 0; i < hm->capacity-1; i++) {
        if (hm->entries[ind].key == NULL) {
            hm->entries[ind].value = value;
            return 1;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return -1;
}