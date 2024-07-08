#include "test_hash.h"
#include "../testing/tassert.h"
#include "../testing/test_utils.h"
#include "hash.h"
#include "out.h"
#include <stdlib.h>

int test_hash() {
    testing_module_setup();

    test_hashing();
    test_init_and_free();
    test_100_inserts();
    test_100_removes();

    testing_module_cleanup();
    return 0;
}

int test_hashing() {
    testing_func_setup();

    char string1[] = "hello";
    char string2[] = "hello";
    const char *key1 = &string1;
    const char *key2 = &string2;
    tassert(key1 != key2);
    tassert(hash(key1) == hash(key2));

    return 0;
}

int test_init_and_free() {
    testing_func_setup();

    HashMap* table = hm_init(10000);
    tassert(table->capacity == 10000);
    hm_free(table);

    return 0;
}

int test_minimal_case() {
    testing_func_setup();

    HashMap* table = hm_init(1);
    const char *key = "hello";
    void *value = (void *)128;
    
    hm_set(table, key, value);
    tassert(hm_lookup(table, key) == (void *)128);

    hm_free(table);
    return 0;
}

int test_100_inserts() {
    testing_func_setup();

    // Allocate the table and string array--it's important that the keys are owned 
    // OUTSIDE the hashtable, as it does not copy them.
    HashMap* table = hm_init(100);
    char **string_array = malloc(100*sizeof(char*));
    for(int i = 0; i<100; i++) {
        string_array[i] = malloc(sizeof(char));
    }
    
    // Add all the elements into the table
    for(int i = 0; i<100; i++) {
        string_array[i][0] = (char)i;
        hm_set(table, string_array[i], (void *)i);
    }

    // Check to see if all of the elements can still be found
    for(int i = 0; i<100; i++) {
        tassert(hm_lookup(table, string_array[i]) == (void *)i);
    }

    // Free. This also frees the string_array
    hm_free(table);
    return 0;
}

int test_100_removes() {
    testing_func_setup();

    // Allocate the table and string array--it's important that the keys are owned 
    // OUTSIDE the hashtable, as it does not copy them.
    HashMap* table = hm_init(100);
    char **string_array = malloc(101*sizeof(char*));
    for(int i = 0; i<101; i++) {
        string_array[i] = malloc(sizeof(char));
        string_array[i][0] = (char)i;
    }
    
    // Add all the elements into the table
    for(int i = 0; i<100; i++) {
        hm_set(table, string_array[i], (void *)i);
    }

    // Remove all the elements from the table
    for(int i = 0; i<100; i++) {
        hm_remove(table, string_array[i]);
    }

    // Check to see if all of the elements are removed
    for(int i = 0; i<100; i++) {
        tassert(hm_lookup(table, string_array[i]) == NULL);
    }

    // check to see that the table is still filled
    tassert(hm_set(table, string_array[100], NULL) == 0);

    // Free. This also frees the string_array
    free(string_array[100]);
    hm_free(table);

    return 0;
}