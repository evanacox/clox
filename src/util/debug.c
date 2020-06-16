#include "debug.h"

#include "disassembler.h"
#include <stdio.h>

void verbose_log(vm *vm) {
    printf("Current stack contents: [  ");

    for (value *slot = vm->stack_top - 1; slot >= vm->stack; --slot) {
        printf("  [ ");
        print_value(*slot);
        printf(" ], ");
    }

    printf("  ]\n");

    disassemble_instruction(vm->chunk, (int)(vm->pc - vm->chunk->code));
}