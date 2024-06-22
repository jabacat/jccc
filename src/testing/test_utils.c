#include <stdio.h>
#include <util/out.h>

void testing_setup_internal(const char *func_name) {
    printf("Running test ");
    fflush(stdout);
    PRINT_WITH_COLOR(CODE_GREEN, "\"%s\"", func_name);
    printf(" ...\n");
}

void testing_cleanup_internal(const char *func_name) {
    printf("Concluded test ");
    fflush(stdout);
    PRINT_WITH_COLOR(CODE_GREEN, "\"%s\"", func_name);
    printf("\n");
}
