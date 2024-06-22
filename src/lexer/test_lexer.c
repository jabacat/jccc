/*
 * Run all of the tests at the end of each C file in the lexer module */

#include "lex.h"

int test_lexer() {
	test_ttype_from_string();
	return 0;
}
