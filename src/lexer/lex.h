#pragma once

#include <stdio.h>

#include "token.h"

// Takes an open file pointer, and a token as input. It fills the token struct
// with the next available token from the file.
int lex(FILE * fp, Token * token);
