/*
 * Run all of the tests at the end of each C file in the lexer module
 */

#include "codegen.h"
#include <testing/test_utils.h>

int test_x86() {
    testing_module_setup();

    test_init_int_literal();

    testing_module_cleanup();
    return 0;
}
