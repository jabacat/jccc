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

int skip_to_token(Lexer *l) {
    char cur;
    char prev;
    if ((cur = fgetc(l->fp)) != EOF) {
        prev = cur;
        if (!(cur == ' ' || cur == '/')) {
            fseek(file, -1, SEEK_CUR);
            return 0; // token begins immediately
        }
    } else {
        return -1; // file done, no more tokens
    }
    // read each character from the file until EOF
    int in_block = 0;
    while ((cur = fgetc(l->fp)) != EOF) {
        if (cur == prev && prev == '/')
            in_block = 1; // single line
        else if (cur == '*' && prev == '/')
            in_block = 2; // block
        else if ((in_block == 1 && cur == '\n') || (in_block == 2 && cur == '/' && prev == '*'))
            in_block = 0; // out of comment

        if (!(cur == ' ' || cur == '/') && in_block == 0) {
            fseek(file, -1, SEEK_CUR);
            return 0; // token is next
        }
        prev = cur;
    }
    return -1;
    // eof was reached.
}
