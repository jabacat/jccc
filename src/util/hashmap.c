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

struct BucketNode *hm_get(void *key);

void hm_set(void *key, void *value);

void double_cap();
