#pragma once

#include <testing/test_utils.h>

enum Op { ADD, SUB, MOV };

char *start_main();

char *end_main();

char *start_func();

char *end_main_custom_return(int val);

char *end_func();

char *init_int_literal(int val);

int test_init_int_literal();

int test_op_on_rax_with_rdi();
