#include "lexer/test_lexer.h"
#include "codegen/x86/test_x86.h"

int main() {
	test_lexer();
	test_x86();

	return 0;
}
