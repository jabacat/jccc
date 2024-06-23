#include <stdint.h>
#include <stdio.h>

struct GenState {
    // Each bit corresponds with a registers 0-31 where the LSB is 0
    uint32_t registers_in_use;

    unsigned int rsp_offset;
} GEN_STATE;

void code_gen_init() {
    GEN_STATE.registers_in_use = 0;
    GEN_STATE.rsp_offset = 0;
}

char *start_main() {
    static char start[256] = "\
global _start\
section .text\
\
_start:";

    return start;
}

char *end_main() {
    static char end[256] = "\
mov rax, 60\
mov rdi, 0\
syscall";

    return end;
}

char *start_func() {
    static char start[256] = "\
sub rsp, 32\
mov [rsp], r12\
mov [rsp+8], r13\
mov [rsp+16], r14\
mov [rsp+24], r15";

    return start;
}

char *end_func() {
    static char end[256] = "\
mov r12, [rsp]\
mov r13, [rsp+8]\
mov r14, [rsp+16]\
mov r15, [rsp+24]\
add rsp, 32";

    return end;
}

char *init_int_literal(int val) {
    GEN_STATE.rsp_offset += 8;

    static char init[256];
    sprintf(init, "mov [rsp+%d] %d", GEN_STATE.rsp_offset, val);

    return init;
}

char *add(int val) {
    GEN_STATE.rsp_offset += 8;

    static char init[256];
    sprintf(init, "mov [rsp+%d] %d", GEN_STATE.rsp_offset, val);

    return init;
}
