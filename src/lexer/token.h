/**
 * This file defines lexer tokens and their structure. The lexer will produce
 * these tokens after scanning a file.
*/

#pragma once

// The type of a token.
typedef enum {
    TT_LITERAL, // a quote, integer, floating-point thing, etc.
    TT_IDENTIFIER, // an identifier, including user-defined types!
    TT_OPAREN, // (
    TT_CPAREN, // )
    TT_OBRACE, // {
    TT_CBRACE, // }
    TT_OBRACKET, // [
    TT_CBRACKET, // ]
    TT_SEMI, // ;
} TokenType;

// The maximum possible length of a token.
#define TOKEN_LENGTH 256

typedef struct {
    TokenType type; // What type of token this is.
    char contents[TOKEN_LENGTH]; // The actual contents of the token.
    unsigned length; // How long the token is.
} Token;
