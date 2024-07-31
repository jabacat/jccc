#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <testing/tassert.h>
#include <testing/test_utils.h>

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

void destroy_hashmap(struct Hashmap *h) {
    free(h->buckets);
    free(h);
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

int hm_set(struct Hashmap *h, void *key, void *value) {
    unsigned a = h->hash(key) % h->cap;

    struct BucketNode *b = h->buckets[a];
    if (b == NULL) {
        h->buckets[a] = malloc(sizeof(struct BucketNode *));
        h->buckets[a]->key = key;
        h->buckets[a]->value = value;
        h->buckets[a]->next = NULL;

        return 0;
    } else {
        // Handle chaining
        return -1;
    }
}

void double_cap(struct Hashmap *h) {
    h->buckets = realloc(h->buckets, h->cap * 2 * sizeof(struct BucketNode *));

    h->size = 0;
    h->cap = h->cap * 2;
}

int test_hash_init() {
    testing_func_setup();
    struct Hashmap *h = create_hashmap(100);

    tassert(h->size == 0);
    tassert(h->cap == 100);
}

int test_hash_init_and_store() {
    testing_func_setup();
    struct Hashmap *h = create_hashmap(100);

    tassert(h->size == 0);
    tassert(h->cap == 100);

    char name[100] = "jake";

    char key[10] = "test";
    int ret = hm_set(h, key, name);
    tassert(ret != -1);

    uint64_t ind = h->hash(key) % h->cap;
    struct BucketNode *b = h->buckets[ind];
    tassert(strcmp(b->key, key) == 0);

    tassert(h->size == 1);
    tassert(h->cap == 100);
}

int test_hash_set_and_get() {
    testing_func_setup();
    struct Hashmap *h = create_hashmap(100);

    char name[100] = "jake";
    char key[10] = "test";

    int ret = hm_set(h, key, name);
    tassert(ret != -1);

    struct BucketNode *got = hm_get(h, "test");
    tassert(strcmp(got->value, "jake") == 0);

    return 0;
}
