#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "out.h"

const uint64_t OFFSET = 14695981039346656037;
const uint64_t FNV_PRIME = 1099511628211;

HashMap* hm_init(int num_buckets) {
    HashMap* hm = malloc(sizeof(HashMap));
    
    // bounce if out of memory
    if (hm == NULL) {
        PRINT_WARNING("out of memory");
        return NULL;
    }

    hm->stored = 0;
    hm->capacity = num_buckets;

    hm->entries = calloc(hm->capacity, sizeof(HashNode));

    // bounce if out of memory
    if (hm->entries == NULL) {
        PRINT_WARNING("out of memory, could not allocate space for %d entries", hm->capacity);
        free(hm->entries);
        return NULL;
    }

    return hm;
}

void hm_free(HashMap *hm) {
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

void* hm_lookup(HashMap *hm, const char* key) {
    uint64_t ind = hash(key) % hm->capacity;
    
    for (int i = 0; i < hm->capacity-1; i++) {
        // since removed entries aren't actually cleared, if you run 
        // into a null key, what you're looking for was never added
        if (hm->entries[ind].key == NULL) {
            return NULL;
        } else if (strcmp(key, hm->entries[ind].key) == 0) {
            return hm->entries[ind].value;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return NULL;
}

int hm_set(HashMap *hm, const char *key, void *value) {
    uint64_t ind = hash(key) % hm->capacity;
    
    for (int i = 0; i < hm->capacity-1; i++) {
        // Values are allowed to be overwritten!
        if ((hm->entries[ind].key == NULL) || (strcmp(key, hm->entries[ind].key) == 0)) {
            hm->entries[ind].value = value;
            hm->entries[ind].key = key;
            return 1;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    PRINT_WARNING("hashmap full!");
    return 0;
}

int hm_remove(HashMap *hm, const char *key) {
    uint64_t ind = hash(key) % hm->capacity;
    
    for (int i = 0; i < hm->capacity-1; i++) {
        if (hm->entries[ind].key == NULL) {
            return 0;
        } else if (strcmp(key, hm->entries[ind].key) == 0) {
            // It just NULL's out the value--this can be written again.
            // that makes hm_lookup return the same thing for a removed
            // item as a non-existent one, though, which is the goal
            hm->entries[ind].value = NULL;
            return 1;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return 0;
}