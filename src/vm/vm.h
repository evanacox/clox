#pragma once

#include "chunk.h"

/**
 * Represents the virtual machine
 */
typedef struct vm {
    /** Pointer to the chunk being executed */
    chunk *chunk;

    /** The program counter */
    uint8_t *pc;
} vm;

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
interpret_result interpret(chunk *chunk);