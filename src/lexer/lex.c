#include "lex.h"
#include "token.h"
#include <assert.h>
#include <stdio.h>
#include <testing/tassert.h> // tassert
#include <testing/test_utils.h>
#include <util/hashmap.h>

#include <ctype.h>
#include <stdint.h>
#include <string.h> // memcpy

#define STREQ(a, b) (!strcmp((a), (b)))

#include <util/out.h> // error reporting

// Is a character in the given string?
int in_string(char c, char s[]) {
    for (char *d = s; *d; ++d) {
        if (*d == c)
            return 1;
    }
    return 0;
}

char single_char_tokens[] = "(){}[];~#,.:?~";

// All strings which represent operators.
char *operator_strings[] = {
    "-",  "+",  "*",  "/",  "=",  ":",  "%",  "&",  "&&",  "|",   "||",  "-=",
    "+=", "++", "--", "/=", "*=", "%=", "&=", "|=", "&&=", "||=", ">",   "<",
    "<=", ">=", "<<", ">>", "!",  "==", "!=", "^",  "^=",  "->",  "<<=", ">>=",
    NULL, // for iterating
};

int starts_operator(char c) {
    switch (c) {
    case '-':
    case '+':
    case '*':
    case '/':
    case '=':
    case ':':
    case '%':
    case '&':
    case '|':
    case '<':
    case '>':
    case '!':
    case '~':
    case '^':
        return 1;
    default:
        return 0;
    }
}

int valid_operator_sequence(char *op) {
    for (char **top = operator_strings; *top; ++top) {
        if (STREQ(*top, op))
            return 1;
    }
    return 0;
}

int is_valid_numeric_or_id_char(char c) {
    return isalnum(c) || (c == '_') || (c == '.');
}

int lexer_getchar(Lexer *l) {
    l->position++;
    l->last_column = l->column;
    l->buffer[0] = getc(l->fp);
    if (l->buffer[0] == '\n') {
        l->line++;
        l->column = 0;
    } else {
        l->column++;
    }
    return l->buffer[0];
}

int lexer_ungetchar(Lexer *l) {
    assert(l->position >= 0);
    l->position--;
    l->column = l->last_column;
    if (l->buffer[0] == '\n') {
        l->line--;
    }
    ungetc(l->buffer[0], l->fp);
    return 1;
}

int real_lex(Lexer *, Token *);

/**
 * This produces a list of tokens after having been processed by the
 * preprocessor. For example, if the code is
 * #define MAX_ARRAY 5
 * int arr[MAX_ARRAY];
 * then this function will return
 * int
 * arr
 * [
 * 5
 * ]
 * ;
 */
int lex(Lexer *l, Token *t) {
    // For now, all we need to do is skip newlines
    for (;;) {
        real_lex(l, t);
        if (t->type != TT_NEWLINE)
            break;
    }
    return 0;
}

// This actually grabs the next token, independent of any preprocessor things.
int real_lex(Lexer *l, Token *t) {
    // If there are any tokens waiting in the putback buffer, read from there
    // instead.
    if (l->unlexed_count > 0) {
        l->unlexed_count--;
        memcpy(t, &l->unlexed[l->unlexed_count], sizeof(Token));
        return 0;
    }

    skip_to_token(l);
    // Get initial character
    int init = lexer_getchar(l);

    // Clear memory and initialize
    memset(t->contents, 0, TOKEN_LENGTH);

    // Set sourcefile
    memcpy(t->source_file, &l->current_file, TOKEN_LENGTH);

    // First important check -- have we reached the end of the file?
    static char eof[] = "[end of file]";
    if (init == EOF) {
        strcpy(t->contents, eof);
        t->length = strlen(eof);
        t->type = TT_EOF;
        t->line = l->line;
        t->column = l->column;
        return 0;
    }

    // Second important check -- are we somehow reading a whitespace character?
    // Not good if so -- report internal error
    if (init == ' ' || init == '\t') {
        PRINT_ERROR("internal error: did not skip whitespace correctly");
        return -1;
    }

    // Last check -- is this a newline?
    static char nline[] = "[newline]";
    if (init == '\n') {
        strcpy(t->contents, nline);
        t->length = strlen(nline);
        t->type = TT_NEWLINE;
        t->line = l->line;
        t->column = l->column;
        return 0;
    }

    // Which position are we writing into?
    int pos = 0;
    // Copy the initial character into the token buffer
    t->contents[pos++] = init;

    /**
     * Because dealing with what type a token is, is not in the scope of this
     * function, we notice that given the initial character of a token, we can
     * immediately tell what character(s) will end said token.
     * For example, if a token begins with an alphabetic character, we read
     * characters until we hit a character that isn't alphanumeric or an
     * underscore.
     * This works for the most part, except that for quote literals we need to
     * make sure the ending quote we hit isn't preceded by a backslash.
     */

    // First up, we can just end here.
    if (in_string(init, single_char_tokens)) {
        t->length = pos;
        t->type = ttype_one_char(init);
        t->line = l->line;
        t->column = l->column;
        return 0;
    }

    // LEXING NUMERIC LITERAL OR IDENTIFIER
    // If it starts with an alphanumeric character or an underscore, search
    // until we hit something which isn't.
    int c;
    int starting_line;
    int starting_col;
    if (is_valid_numeric_or_id_char(init)) {
        starting_line = l->line;
        starting_col = l->column;
        for (;;) {
            c = lexer_getchar(l);
            // If not alphanumeric or underscore, skip to end
            if (!is_valid_numeric_or_id_char(c))
                break;
            // OOB check
            if (pos >= TOKEN_LENGTH - 1) {
                PRINT_ERROR("identifier too long, over %d characters",
                            TOKEN_LENGTH);
                PRINT_ERROR("identifier began with the following:");
                PRINT_ERROR("%.*s", TOKEN_LENGTH, t->contents);
                return -1;
            }
            t->contents[pos++] = c;
        }
        // We've ended!
        lexer_ungetchar(l);
        t->contents[pos] = '\0';
        t->type = ttype_many_chars(t->contents);
        t->length = pos;
        t->line = starting_line;
        t->column = starting_col;
        return 0;
    }

    // Lex an operator. We do this by lexing characters from the buffer until
    // the resulting string is no longer an operator, then we cut our losses and
    // return./
    if (starts_operator(init)) {
        while (valid_operator_sequence(t->contents)) {
            t->contents[pos++] = (c = lexer_getchar(l));
        }
        // We've ended!
        // Can we reduce this code duplication from above in a smart way?
        lexer_ungetchar(l);
        t->contents[pos - 1] = '\0';
        t->type = ttype_from_string(t->contents);
        t->length = pos;
        return 0;
    }

    // TODO - parse character or string literal

    PRINT_ERROR("lexer unable to identify token starting with: %c", init);
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

int skip_to_token(Lexer *l) {
    char cur, prev;
    int in_block = 0, pass = 0;

    // Read the first character
    if ((cur = lexer_getchar(l)) != EOF) {
        prev = cur;
        if (!(cur == ' ' || cur == '\t' || cur == '/')) {
            lexer_ungetchar(l);
            return 0; // Token begins immediately
        }
    } else {
        return -1; // File done, no more tokens
    }

    // Read each character from the file until EOF
    while ((cur = lexer_getchar(l)) != EOF) {
        if (cur == '/' && prev == '/' && in_block == 0) {
            in_block = 1; // Single line comment
        } else if (cur == '*' && prev == '/' && in_block == 0) {
            in_block = 2; // Block comment
            pass = 2;
        } else if ((in_block == 1 && cur == '\n') ||
                   (in_block == 2 && cur == '/' && prev == '*' && pass <= 0)) {
            in_block = 0; // Out of comment
        } else if (prev == '/' && !(cur == '*' || cur == '/') &&
                   in_block == 0) {
            lexer_ungetchar(l);
            return 0; // Token was a slash without a * or / following it
        }
        if (!(cur == ' ' || cur == '\t' || cur == '/') && in_block == 0) {
            lexer_ungetchar(l);
            return 0; // Token is next
        }

        pass -= 1;
        prev = cur;
    }

    return -1; // EOF was reached
}

// This is a function for parsing single char tokens
// Now handles all cases of single char tokens
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
    case '.':
        return TT_PERIOD; // .
    case ',':
        return TT_COMMA; // ,
    case '-':
        return TT_MINUS; // -
    case '+':
        return TT_PLUS; // +
    case '*':
        return TT_STAR; // *
    case '/':
        return TT_SLASH; // /
    case '=':
        return TT_ASSIGN; // =
    case ':':
        return TT_COLON; // :
    case '%':
        return TT_MOD; // %
    case '&':
        return TT_BAND; // &
    case '|':
        return TT_BOR; // |
    case '>':
        return TT_GREATER; // >
    case '<':
        return TT_LESS; // <
    case '!':
        return TT_LNOT; // !
    case '~':
        return TT_BNOT; // ~
    case '^':
        return TT_XOR; // ^
    case '#':
        return TT_POUND;
    case '?':
        return TT_QMARK;
    default:
        if (isdigit(c)) {
            return TT_LITERAL;
        } else {
            return TT_IDENTIFIER;
        }
    }
}

char *keyword_and_operator_strings[] = {
    "auto",     "break",   "continue", "const", "case",   "char",     "do",
    "double",   "default", "enum",     "else",  "extern", "float",    "for",
    "goto",     "int",     "if",       "long",  "return", "register", "struct",
    "signed",   "sizeof",  "static",   "short", "switch", "typedef",  "union",
    "unsigned", "void",    "volatile", "while", "&&",     "||",       "-=",
    "+=",       "++",      "--",       "/=",    "*=",     "%=",       "&=",
    "|=",       "&&=",     "||=",      "<=",    ">=",     "<<",       ">>",
    "==",       "^=",      "->",       "<<=",   ">>=",    "!=",
};

TokenType keyword_token_types[] = {
    TT_AUTO,
    TT_BREAK,
    TT_CONTINUE,
    TT_CONST,
    TT_CASE,
    TT_CHAR,
    TT_DO,
    TT_DOUBLE,
    TT_DEFAULT,
    TT_ENUM,
    TT_ELSE,
    TT_EXTERN,
    TT_FLOAT,
    TT_FOR,
    TT_GOTO,
    TT_INT,
    TT_IF,
    TT_LONG,
    TT_RETURN,
    TT_REGISTER,
    TT_STRUCT,
    TT_SIGNED,
    TT_SIZEOF,
    TT_STATIC,
    TT_SHORT,
    TT_SWITCH,
    TT_TYPEDEF,
    TT_UNION,
    TT_UNSIGNED,
    TT_VOID,
    TT_VOLATILE,
    TT_WHILE,
    TT_LAND,
    TT_LOR,
    TT_DEC,
    TT_INC,
    TT_PLUSPLUS,
    TT_MINUSMINUS,
    TT_DIVEQ,
    TT_MULEQ,
    TT_MODEQ,
    TT_BANDEQ,
    TT_BOREQ,
    TT_LANDEQ,
    TT_LOREQ,
    TT_LESSEQ,
    TT_GREATEREQ,
    TT_LEFTSHIFT,
    TT_RIGHTSHIFT,
    TT_EQUALS,
    TT_XOREQ,
    TT_POINT,
    TT_LEFTSHIFTEQUALS,
    TT_RIGHTSHIFTEQUALS,
    TT_NOTEQ,
};

unsigned int perfect_keyword_hash_function(char *keyword) {
    // (ord(key[0]) + ord(key[1]) * 2 + ord(key[-1]) + ord(key[-2]) ** 2) % 406
    int len = strlen(keyword);

    if (keyword == NULL) {
        return -1;
    }

    int a = (keyword[0] + keyword[1] * 2 + keyword[len - 1] +
             keyword[len - 2] * keyword[len - 2]) %
            406;
}

struct Hashmap *init_keyword_hashmap() {
    struct Hashmap *h = create_hashmap(406);
    h->hash = perfect_keyword_hash_function;

    for (int i = 0; i < 55; ++i) {
        char *keyword = keyword_and_operator_strings[i];
        int ret = hm_set(h, keyword, (void *)(uintptr_t)keyword_token_types[i]);
        struct BucketNode *b = hm_get(h, keyword);

        assert((TokenType)(uintptr_t)b->value == keyword_token_types[i]);
    }

    return h;
}

struct Hashmap *KEYWORD_HASHMAP = NULL;

// This is a function for parsing exclusively tokens with more than one char
TokenType ttype_many_chars(const char *contents) {
    if (KEYWORD_HASHMAP == NULL) {
        KEYWORD_HASHMAP = init_keyword_hashmap();
    }

    struct BucketNode *b = hm_get(KEYWORD_HASHMAP, contents);
    if (b != NULL) {
        assert(b->key == contents);
        return (TokenType)(uintptr_t)b->value;
    }

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
        if ((c > '9' || c < '0') && c != 'u') {
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

// This is the function for parsing all tokens from strings
TokenType ttype_from_string(const char *contents) {
    int len;

    len = strlen(contents);

    // Single character contents
    if (len == 1) {
        TokenType token = ttype_one_char(contents[0]);
        return token;
    }

    return ttype_many_chars(contents);
}

static const char *ttype_names[] = {
    "literal",       // a quote, integer, floating-point thing, etc.
    "identifier",    // an identifier, including user-defined types!
    "open paren",    // (
    "close paren",   // )
    "open brace",    // {
    "close brace",   // }
    "open bracket",  // [
    "close bracket", // ]
    "semicolon",     // ;
    "no token",      // Not a token
    "end of file",   // End-of-file, lex until we hit the end of the file
    "newline",       // Newline, used in preprocessing
    "pound",
    ".",
    ",",
    "?",
    "-",
    "+",
    "*",
    "/",
    "=",
    ":",
    "%",
    "&",
    "&&",
    "|",
    "||",
    "-=",
    "+=",
    "++",
    "--",
    "/=",
    "*=",
    "%=",
    "&=",
    "|=",
    "&&=",
    "||=",
    ">",
    "<",
    "<=",
    ">=",
    "<<",
    ">>",
    "!",
    "~",
    "==",
    "!=",
    "^",
    "^=",
    "->",
    "<<=",
    ">>=",
    // All 32 C keywords
    "auto",
    "break",
    "char",
    "const",
    "case",
    "continue",
    "double",
    "do",
    "default",
    "enum",
    "else",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "return",
    "register",
    "static",
    "switch",
    "short",
    "signed",
    "struct",
    "sizeof",
    "typedef",
    "unsigned",
    "union",
    "void",
    "volatile",
    "while",
};

const char *ttype_name(TokenType tt) { return ttype_names[tt]; }

int test_keyword_hash() {
    testing_func_setup();

    struct Hashmap *h = init_keyword_hashmap();

    return 0;
}

int test_ttype_many_chars() {
    testing_func_setup();

    tassert(ttype_many_chars("foo") == TT_IDENTIFIER);
    tassert(ttype_many_chars("struct") == TT_STRUCT);
    tassert(ttype_many_chars("while") == TT_WHILE);

    return 0;
}

int test_ttype_one_char() {
    testing_func_setup();

    // Use ttype_from_string
    tassert(ttype_one_char('a') == TT_IDENTIFIER);
    tassert(ttype_one_char('1') == TT_LITERAL);

    tassert(ttype_one_char('+') == TT_PLUS);
    tassert(ttype_one_char('-') == TT_MINUS);
    tassert(ttype_one_char('>') == TT_GREATER);
    tassert(ttype_one_char('~') == TT_BNOT);

    return 0;
}

int test_ttype_name() {
    testing_func_setup();

    tassert(strcmp(ttype_name(TT_CHAR), "char") == 0);
    tassert(strcmp(ttype_name(TT_FOR), "for") == 0);
    tassert(strcmp(ttype_name(TT_LITERAL), "literal") == 0);
    tassert(strcmp(ttype_name(TT_PLUS), "+") == 0);
    tassert(strcmp(ttype_name(TT_STRUCT), "struct") == 0);
    tassert(strcmp(ttype_name(TT_VOLATILE), "volatile") == 0);
    tassert(strcmp(ttype_name(TT_SIZEOF), "sizeof") == 0);
    tassert(strcmp(ttype_name(TT_WHILE), "while") == 0);

    return 0;
}

int test_ttype_from_string() {
    testing_func_setup();

    tassert(ttype_from_string("+") == TT_PLUS);
    tassert(ttype_from_string("=") == TT_ASSIGN);

    tassert(ttype_from_string("1") == TT_LITERAL);
    tassert(ttype_from_string("1.2") == TT_LITERAL);

    tassert(ttype_from_string("1u") == TT_LITERAL);
    tassert(ttype_from_string("1.2f") == TT_LITERAL);
    tassert(ttype_from_string("1.f") == TT_LITERAL);

    tassert(ttype_from_string("\"Planck\"") == TT_LITERAL);
    tassert(ttype_from_string("'Language'") == TT_LITERAL);

    tassert(ttype_from_string("Jaba") == TT_IDENTIFIER);
    tassert(ttype_from_string("cat_") == TT_IDENTIFIER);

    tassert(ttype_from_string("(") == TT_OPAREN);
    tassert(ttype_from_string("}") == TT_CBRACE);

    tassert(ttype_from_string(";") == TT_SEMI);

    return 0;
}
