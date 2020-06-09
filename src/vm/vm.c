#include "vm.h"
#include "../common/memory.h"
#include <stdio.h>
#include <stdlib.h>

static vm s_vm;

static interpret_result run() {
    while (true) {
        uint8_t instr = 0;
        switch (instr = (*s_vm.pc++)) {
            case OP_RETURN: {
                return INTERPRET_OK;
            }
            case OP_LOAD_CONST: {
                value constant = s_vm.chunk->constant_pool.values[(*s_vm.pc++)];
                print_value(constant);
                printf("\n");
                break;
            }
            case OP_LOAD_CONST_LONG: {
                size_t const_pool_idx =
                    from_bytes(*s_vm.pc++, *s_vm.pc++, *s_vm.pc++);
                
                value constant =
                    s_vm.chunk->constant_pool.values[const_pool_idx];
                print_value(constant);
                break;
            }
        }
    }
}

void init_vm() { s_vm.chunk = NULL; }

void free_vm() {}

interpret_result interpret(chunk *chunk) {
    s_vm.chunk = chunk;
    s_vm.pc = s_vm.chunk->code;

    return run();
}