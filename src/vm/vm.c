#include "vm.h"
#include "../common/memory.h"
#include "../compiler/compiler.h"
#include "../util/debug.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG_TRACE
#include "../util/disassembler.h"
#endif

static vm s_vm;

/**
 * "Resets" the stack to the stack, effectively hides all the values
 * that are there
 */
static inline void reset_stack() {
    s_vm.stack_top = s_vm.stack;
}

/**
 * Returns the stack value `distance` elements from the top of the stack
 * @param distance The number of elements backwards from the stack top
 * @return The value at that point on the stack
 */
static inline value peek(size_t distance) {
    return s_vm.stack_top[-1 - distance];
}

/**
 * Format-able error printer
 * @param format The format string
 * @param ... Any format arguments
 */
static inline void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
    fputs("\n", stderr);

    size_t instr = s_vm.pc - s_vm.chunk->code - 1;
    size_t line = s_vm.chunk->lines[instr];

    fprintf(stderr, "[line #%zu] in script\n", line);

    reset_stack();
}

/**
 * Returns if a value is "falsey" (meaning `nil` or `false`
 * @param val The value to check
 * @return True if the object is falsey, false otherwise
 */
static inline bool is_falsey(value val) {
    return is_nil(val) || (is_bool(val) && !as_bool(val));
}

static interpret_result run() {
#define BINARY_OP(type, op)                                                                        \
    do {                                                                                           \
        if (!is_number(peek(0)) || !is_number(peek(1))) {                                          \
            runtime_error("Operands for operator#op must be numbers.");                            \
            return INTERPRET_RUNTIME_ERROR;                                                        \
        }                                                                                          \
        double b = as_number(pop());                                                               \
        double a = as_number(pop());                                                               \
        push(type(a op b));                                                                        \
    } while (false)

    while (true) {
#ifdef DEBUG_TRACE
        verbose_log(&s_vm);
#endif
        switch (*s_vm.pc++) {
            case OP_LOAD_CONST: push(s_vm.chunk->constant_pool.values[*s_vm.pc++]); break;
            case OP_ADD: BINARY_OP(number_value, +); break;
            case OP_SUBTRACT: BINARY_OP(number_value, -); break;
            case OP_MULTIPLY: BINARY_OP(number_value, *); break;
            case OP_DIVIDE: BINARY_OP(number_value, /); break;
            case OP_EQUAL: push(bool_value(are_equal(pop(), pop()))); break;
            case OP_GREATER: BINARY_OP(bool_value, >); break;
            case OP_LESS: BINARY_OP(bool_value, <); break;
            case OP_NOT: push(bool_value(is_falsey(pop()))); break;
            case OP_NIL: push(nil_value()); break;
            case OP_TRUE: push(bool_value(true)); break;
            case OP_FALSE: push(bool_value(false)); break;
            case OP_RETURN:
                print_value(pop());
                printf("\n");
                return INTERPRET_OK;
            case OP_NEGATE:
                if (!is_number(peek(0))) {
                    runtime_error("Operand to operator- must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                push(number_value(-as_number(pop())));
                break;
            case OP_LOAD_CONST_LONG: {
                size_t bytes = from_bytes(*s_vm.pc++, *s_vm.pc++, *s_vm.pc++);
                push(s_vm.chunk->constant_pool.values[bytes]);
                break;
            }
        }
    }

#undef BINARY_OP
}

void init_vm() {
    reset_stack();
    s_vm.chunk = NULL;
}

void free_vm() {
}

interpret_result interpret(const char *source) {
    chunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    s_vm.chunk = &chunk;
    s_vm.pc = s_vm.chunk->code;

    interpret_result res = run();
    free_chunk(&chunk);
    return res;
}

void push(value v) {
    *s_vm.stack_top = v;
    ++s_vm.stack_top;
}

value pop() {
    --s_vm.stack_top;
    return *s_vm.stack_top;
}