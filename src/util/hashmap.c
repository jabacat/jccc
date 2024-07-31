#include "hashmap.h"
#include <stdlib.h>

unsigned fnva1(char *value) {
    unsigned long long h = 14695981039346656037;
    long int prime = 1099511628211;

    while (value) {
        h ^= prime;
        (*value)++;
    }

    return h;
}

struct Hashmap *create_hashmap(int capacity) {
    struct Hashmap *h = malloc(sizeof(struct Hashmap));

    h->buckets = calloc(capacity, sizeof(struct BucketNode *));

    h->size = 0;
    h->cap = capacity;

    return h;
}

struct BucketNode *create_bucket(void *key, void *value) {
    struct BucketNode *b = malloc(sizeof(struct BucketNode));

    b->key = key;
    b->value = value;
    b->next = NULL;

    return b;
}

struct BucketNode *hm_get(struct Hashmap *h, void *key) {
    unsigned a = h->hash(key) % h->cap;

    struct BucketNode *b = h->buckets[a];
    if (b != NULL) {

        // Check if key is the same, because the hash might have collided
        while (!h->equals(key, b->key)) {
            b = b->next;
        }
        return b;
    }

    return NULL;
}

void hm_set(struct Hashmap *h, void *key, void *value) {
    unsigned a = h->hash(key) % h->cap;

    struct BucketNode *b = h->buckets[a];
    if (b == NULL) {
        h->buckets[a] = malloc(sizeof(struct BucketNode *));
        h->buckets[a]->key = key;
        h->buckets[a]->value = value;
        h->buckets[a]->next = NULL;
    } else {
        // Handle chaining
    }
}

void double_cap(struct Hashmap *h) {
    h->buckets = realloc(h->buckets, h->cap * 2 * sizeof(struct BucketNode *));

    h->size = 0;
    h->cap = h->cap * 2;
}
