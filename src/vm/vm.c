#include "vm.h"
#include "../common/memory.h"
#include "../compiler/compiler.h"
#include "../util/debug.h"
#include "object.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_TRACE
#include "../util/disassembler.h"
#endif

vm g_vm;

/**
 * "Resets" the stack to the stack, effectively hides all the values
 * that are there
 */
static inline void reset_stack() {
    g_vm.stack_top = g_vm.stack;
}

/**
 * Returns the stack value `distance` elements from the top of the stack
 * @param distance The number of elements backwards from the stack top
 * @return The value at that point on the stack
 */
static inline value peek(size_t distance) {
    return g_vm.stack_top[-1 - distance];
}

/**
 * Returns if a value is "falsey" (meaning `nil` or `false`
 * @param val The value to check
 * @return True if the object is falsey, false otherwise
 */
static inline bool is_falsey(value val) {
    return is_nil(val) || (is_bool(val) && !as_bool(val));
}

/**
 * Frees all the Lox objects after the program ends
 */
static void free_objects() {
    object *obj = g_vm.objects;

    while (obj != NULL) {
        object *next = obj->next;
        free_object(obj);
        obj = next;
    }
}

/**
 * Format-able error printer
 * @param format The format string
 * @param ... Any format arguments
 */
static void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
    fputs("\n", stderr);

    size_t instr = g_vm.pc - g_vm.chunk->code - 1;
    size_t line = g_vm.chunk->lines[instr];

    fprintf(stderr, "[line #%zu] in script\n", line);

    reset_stack();
}

/**
 * Concatenates two String objects and pushes the result onto the stack
 */
static void concatenate() {
    string *b = as_string(pop());
    string *a = as_string(pop());

    int new_length = a->len + b->len;
    char *new_string = ALLOCATE(char, new_length + 1);
    memcpy(new_string, a->chars, a->len);
    memcpy(new_string + a->len, b->chars, b->len);
    new_string[new_length] = '\0';

    string *res = from_string(new_string, new_length);
    push(object_value((object *)res));
}

/**
 * Runs the chunk and returns the result
 * @return The result of the interpretation
 */
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
        switch (*g_vm.pc++) {
            case OP_LOAD_CONST: push(g_vm.chunk->constant_pool.values[*g_vm.pc++]); break;
            case OP_ADD: {
                if (is_string(peek(0)) && is_string(peek(1))) {
                    concatenate();
                } else if (is_number(peek(0)) && is_number(peek(1))) {
                    double b = as_number(pop());
                    double a = as_number(pop());

                    push(number_value(a + b));
                } else {
                    runtime_error("Operands for operator#op must be numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                break;
            }
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
                size_t bytes = from_bytes(*g_vm.pc++, *g_vm.pc++, *g_vm.pc++);
                push(g_vm.chunk->constant_pool.values[bytes]);
                break;
            }
        }
    }

#undef BINARY_OP
}

void init_vm() {
    reset_stack();
    g_vm.chunk = NULL;
    g_vm.objects = NULL;
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

    g_vm.chunk = &chunk;
    g_vm.pc = g_vm.chunk->code;

    interpret_result res = run();
    free_chunk(&chunk);
    return res;
}

void push(value v) {
    *g_vm.stack_top = v;
    ++g_vm.stack_top;
}

value pop() {
    --g_vm.stack_top;
    return *g_vm.stack_top;
}