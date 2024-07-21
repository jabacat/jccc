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
    unsigned (*equals)(void *);
};

struct BucketNode *create_bucket(void *key, void *value);

// Get a value with a key
struct BucketNode *hm_get(void *key);
// Set a value with a key
void hm_set(void *key, void *value);
// Double the capacity of the hashmap (happens automatically when size >
// capacity)
void double_cap();
