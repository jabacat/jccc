#pragma once

#include <stdio.h>

#include "token.h"

// This is a lexer structure, which will contain all of the information about
// the state of a lexer. For now all we need is the file pointer which actually
// has the data we need, but in the future we will need more.
typedef struct {
    FILE * fp;
} Lexer;

// Takes an open file pointer, and a token as input. It fills the token struct
// with the next available token from the file.
int lex(Lexer * l, Token * token);
