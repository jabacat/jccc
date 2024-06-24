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

void* lget_element(List* l, int index) {
    int i = index;
    ListBlock* lb = l->head;
    if (index < 0) {
        PRINT_ERROR("internal: list index was negative (%d)", index);
        return NULL;
    }
    // We see if the index is in range of the current block length. If not,
    // repeat with the next block.
    while (i >= lb->size) {
        if (lb) {
            lb = lb->next;
            i -= lb->size;
        } else {
            PRINT_ERROR("internal: list index %d out of bounds", index);
            return NULL;
        }
    }
    // Now the index is in range and we are at the list block we wish to access.
    if (i >= lb->full) {
        PRINT_ERROR("internal: list index %d out of bounds", index);
        return NULL;
    }
    return lb->array[i];
}
