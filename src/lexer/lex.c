#include "lex.h"

#include <string.h> // memcpy

int lex(Lexer *l, Token *t) {
    // If there are any tokens waiting in the putback buffer, read from there
    // instead.
    if (l->unlexed_count > 0) {
        l->unlexed_count--;
        memcpy(t, &l->unlexed[l->unlexed_count], sizeof(Token));
        return 0;
    }
    // Now, do the actual lexing: TODO!
    return 0;
}

int unlex(Lexer *l, Token *t) {
    // First, make sure we can actually fit it in the buffer.
    if (l->unlexed_count >= TOKEN_PUTBACKS) {
        // TODO: error reporting
        return -1; // Error return code
    }
    memcpy(&l->unlexed[l->unlexed_count], t, sizeof(Token));
    l->unlexed_count++;
    return 0;
}
