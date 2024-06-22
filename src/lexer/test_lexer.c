/*
 * Run all of the tests at the end of each C file in the lexer module
 */

#include "lex.h"
#include <testing/test_utils.h>

int test_lexer() {
    testing_setup();

    test_ttype_from_string();

    testing_cleanup();
    return 0;
}
