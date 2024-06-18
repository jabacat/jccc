#pragma once

#include <stdio.h>

#include "token.h"

// The maximum number of tokens we can put back.
#define TOKEN_PUTBACKS 5

// This is a lexer structure, which will contain all of the information about
// the state of a lexer.
typedef struct {
    FILE * fp; // The file we are reading from.
    Token unlexed[TOKEN_PUTBACKS];
    unsigned unlexed_count;
} Lexer;

// Takes an open file pointer, and a token as input. It fills the token struct
// with the next available token from the file.
int lex(Lexer * l, Token * token);

// Put a token back to be lexed again in the future.
int unlex(Lexer * l, Token * token);

// Find token type for single char
TokenType ttype_one_char(char c);

// Find token type for a char* with multiple characters
TokenType ttype_many_chars(const char *contents);

// Discern the token type from the given contents of unknown length
TokenType ttype_from_string(const char *contents);

// Jump to next token by skipping whitespace and comments (doesn't read it)
int skip_to_token(Lexer *l);
