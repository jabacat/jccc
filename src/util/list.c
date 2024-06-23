#include "list.h"

#include <stdlib.h> // memory

#include <util/out.h>

typedef struct ListBlock {
    void* * array;
    int size, full;
    struct ListBlock* next;
} ListBlock;

struct List {
    ListBlock *head, *tail;
    int blocksize;
};

List* create_list(int blocksize) {
    List* l = malloc(sizeof(List));
    if (!l) {
        PRINT_ERROR("internal: failed to allocate memory");
        return NULL;
    }
    l->blocksize = blocksize;
    l->head = l->tail = NULL;
}

int destroy_list(List* l) {
    // Free all blocks.
    ListBlock *lb = l->head, *next;
    while (lb) {
        next = lb->next;
        free(lb->array);
        lb = next;
    }
    free(l);
    return 0;
}
