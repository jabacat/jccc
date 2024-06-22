#include "lex.h"

#include <assert.h> // assert
#include <string.h> // memcpy

#define STREQ(a, b) (!strcmp((a), (b)))

#include <util/out.h> // error reporting

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
        PRINT_ERROR("internal: tried to unlex more than %d tokens at a time",
                    TOKEN_PUTBACKS);
        return -1; // Error return code
    }
    memcpy(&l->unlexed[l->unlexed_count], t, sizeof(Token));
    l->unlexed_count++;
    return 0;
}

TokenType ttype_one_char(char c) {
    switch (c) {
    case '(':
        return TT_OPAREN; // (
    case ')':
        return TT_CPAREN; // )
    case '{':
        return TT_OBRACE; // {
    case '}':
        return TT_CBRACE; // }
    case '[':
        return TT_OBRACKET; // [
    case ']':
        return TT_CBRACKET; // ]
    case ';':
        return TT_SEMI; // ;
    }

    return TT_NO_TOKEN;
}

TokenType ttype_many_chars(const char *contents) {
    if (STREQ(contents, "auto")) {
        return TT_AUTO;
    } else if (STREQ(contents, "break")) {
        return TT_BREAK;
    } else if (STREQ(contents, "continue")) {
        return TT_CONTINUE;
    } else if (STREQ(contents, "const")) {
        return TT_CONST;
    } else if (STREQ(contents, "case")) {
        return TT_CASE;
    } else if (STREQ(contents, "char")) {
        return TT_CHAR;
    } else if (STREQ(contents, "do")) {
        return TT_DO;
    } else if (STREQ(contents, "double")) {
        return TT_DOUBLE;
    } else if (STREQ(contents, "default")) {
        return TT_DEFAULT;
    } else if (STREQ(contents, "enum")) {
        return TT_ENUM;
    } else if (STREQ(contents, "else")) {
        return TT_ELSE;
    } else if (STREQ(contents, "extern")) {
        return TT_EXTERN;
    } else if (STREQ(contents, "float")) {
        return TT_FLOAT;
    } else if (STREQ(contents, "for")) {
        return TT_FOR;
    } else if (STREQ(contents, "goto")) {
        return TT_GOTO;
    } else if (STREQ(contents, "int")) {
        return TT_INT;
    } else if (STREQ(contents, "if")) {
        return TT_IF;
    } else if (STREQ(contents, "long")) {
        return TT_LONG;
    } else if (STREQ(contents, "return")) {
        return TT_RETURN;
    } else if (STREQ(contents, "register")) {
        return TT_REGISTER;
    } else if (STREQ(contents, "struct")) {
        return TT_STRUCT;
    } else if (STREQ(contents, "signed")) {
        return TT_SIGNED;
    } else if (STREQ(contents, "sizeof")) {
        return TT_SIZEOF;
    } else if (STREQ(contents, "static")) {
        return TT_STATIC;
    } else if (STREQ(contents, "short")) {
        return TT_SHORT;
    } else if (STREQ(contents, "switch")) {
        return TT_SWITCH;
    } else if (STREQ(contents, "typedef")) {
        return TT_TYPEDEF;
    } else if (STREQ(contents, "union")) {
        return TT_UNION;
    } else if (STREQ(contents, "unsigned")) {
        return TT_UNSIGNED;
    } else if (STREQ(contents, "void")) {
        return TT_SIZEOF;
    } else if (STREQ(contents, "volitile")) {
        return TT_SIZEOF;
    } else if (STREQ(contents, "while")) {
        return TT_WHILE;
    }

    // TODO: Handle operations

    // Includes only numbers
    int all_numeric = 1;

    int count_fs = 0;
    int count_us = 0;

    int i;

    if (contents == NULL) {
        return TT_NO_TOKEN;
    }

    // Loop through each character
    for (i = 0; contents[i] != '\0'; i++) {
        char c = contents[i];

        // If it has a period, it's a float
        if (c == '.') {
            return TT_LITERAL;
        }

        // Count 'f'
        if (c == 'f') {
            count_fs++;
        }

        // Count 'u'
        if (c == 'u') {
            count_us++;
        }

        // Is it from "0123456789"?
        if (c > '9' || c < '0' && c != 'u') {
            all_numeric = 0;
        }

        if (c == '\'' || c == '"') {
            return TT_LITERAL;
        }
    }

    if (all_numeric) {
        // 100u
        if (count_us == 1 && contents[i - 1] == 'u') {
            return TT_LITERAL;
        }

        // 100
        if (count_us == 0) {
            return TT_LITERAL;
        }
    }

    return TT_IDENTIFIER;
}

TokenType ttype_from_string(const char *contents) {
    int len;

    len = strlen(contents);

    // Single character contents
    if (len == 1) {
        TokenType token = ttype_one_char(contents[0]);

        if (token != TT_NO_TOKEN) {
            return token;
        }
    }

    return ttype_many_chars(contents);
}

int test_ttype_from_string() {
    assert(ttype_from_string("1") == TT_LITERAL);
    assert(ttype_from_string("1.2") == TT_LITERAL);

    assert(ttype_from_string("1u") == TT_LITERAL);
    assert(ttype_from_string("1.2f") == TT_LITERAL);
    assert(ttype_from_string("1.f") == TT_LITERAL);

    assert(ttype_from_string("\"Planck\"") == TT_LITERAL);
    assert(ttype_from_string("'Language'") == TT_LITERAL);

    assert(ttype_from_string("Jaba") == TT_IDENTIFIER);
    assert(ttype_from_string("cat_") == TT_IDENTIFIER);

    assert(ttype_from_string("(") == TT_OPAREN);
    assert(ttype_from_string("}") == TT_CBRACE);

    assert(ttype_from_string(";") == TT_SEMI);

    return 0;
}
