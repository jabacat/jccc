/**
 * Main C source code file for the JabaCat C Compiler.
 */

#include <stdio.h>

#include <lexer/lex.h>
#include <util/out.h>

#define TEST_FILE "tests/lextest.c"

int main(int argc, char **argv) {
    
    // Initialization of everything
    Lexer lexer;
    FILE * fp = fopen(TEST_FILE, "r");
    if (!fp) {
        PRINT_ERROR("File %s not found", TEST_FILE);
        return 1;
    }
    lexer.fp = fp;
    lexer.unlexed_count = 0;

    Token t;
    do {
        lex(&lexer, &t);
        printf("Contents: %20s, type: %20s\n", t.contents, ttype_name(t.type));
    } while (t.type != TT_EOF);

    return 0;
}
