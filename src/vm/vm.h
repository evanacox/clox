#pragma once

#include "chunk.h"
#include "object.h"

#define MAX_STACK_SIZE 256

/**
 * Represents the virtual machine
 */
typedef struct vm {
    /** Pointer to the chunk being executed */
    chunk *chunk;

    /** The program counter */
    uint8_t *pc;

    /** The "stack" for the VM */
    value stack[MAX_STACK_SIZE];

    /** Stack pointer */
    value *stack_top;

    /** Pointer to the linked list of objects */
    object *objects;
} vm;

extern vm g_vm;

/**
 * Represents a result of an interpretation
 */
typedef enum interpret_result {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} interpret_result;

/**
 * Initializes the virtual machine
 */
void init_vm();

/**
 * Leaves the VM in a state ready for deletion
 */
void free_vm();

/**
 * Interprets a chunk
 * @param chunk The chunk to interpret
 * @return The result of the interpretation
 */
interpret_result interpret(const char *source);

/**
 * Pushes a value onto the VM's stack
 * @param val The value to push
 */
void push(value val);

/**
 * Pops a value from the VM's stack
 * @return The value at the top of the stack
 */
value pop();