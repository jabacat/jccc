struct BucketNode {
    void *key;
    void *value;
    struct BucketNode *next;
};

struct Hashmap {
    struct BucketNode **buckets;
    int size;
    int cap;

    unsigned (*hash)(void *);
    unsigned (*equals)(void *, void *);
};

struct Hashmap *create_hashmap(int capacity);
void destroy_hashmap(struct Hashmap *h);

struct BucketNode *create_bucket(void *key, void *value);

// Get a value with a key
struct BucketNode *hm_get(struct Hashmap *h, void *key);
// Set a value with a key
void hm_set(struct Hashmap *h, void *key, void *value);
// Double the capacity of the hashmap (happens automatically when size >
// capacity)
void double_cap(struct Hashmap *h);
