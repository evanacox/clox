#pragma once

#include "../common/common.h"
#include "value.h"
#include <assert.h>

/**
 * @brief Represents an chunk's "operation code"
 * @details An enum that represents which opcode the current chunk is
 */
typedef enum op_code {
    OP_RETURN,
    OP_LOAD_CONST,
    OP_LOAD_CONST_LONG,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
} __attribute__((__packed__)) op_code;

_Static_assert(sizeof(op_code) == sizeof(uint8_t), "op_code should be same size as byte");

/**
 * @brief Represents a single VM chunk
 * @details Holds the bytes for the chunk and some other information
 */
typedef struct chunk {
    /** The total number of bytes in the byte array */
    size_t size;

    /** The length of the byte array */
    size_t capacity;

    /** Pointer to the raw bytes of the chunk */
    uint8_t *code;

    /** Pool of all the constant values for the chunk */
    value_array constant_pool;

    /**
     * Line that each bytecode instruction corresponds to in the source, encoded
     * in run-length encoding
     */
    size_t *lines;

    /** The number of elements in lines */
    size_t lines_size;

    /** The total capacity of the line array */
    size_t lines_capacity;
} chunk;

/**
 * Initializes an chunk with a capacity and size of 0.
 * @param chunk The chunk to initialize
 */
void init_chunk(chunk *chunk);

/**
 * Initializes an chunk
 * @param chunk The chunk to initialize
 * @param capacity The initial size for the byte array
 */
void init_chunk_with_size(chunk *chunk, size_t capacity);

/**
 * Writes a byte to the chunk's code segment
 * @param chunk The chunk to write to
 * @param byte The byte to append
 * @param line The line that this byte should be associated with
 */
void write_byte(chunk *chunk, uint8_t byte, size_t line);

/**
 * Writes a CONST_LOAD instruction
 * @param chunk The chunk to write to
 * @param constant The constant being loaded
 * @param line The line the instruction should be associated with
 */
void write_constant(chunk *chunk, value constant, size_t line);

/**
 * Adds a constant to the chunk's value_array
 * @param chunk The chunk to add the constant to
 * @param constant The constant to add
 * @returns The index of the new constant in the array
 */
int add_constant(chunk *chunk, value constant);

/**
 * Frees an chunk
 * @param chunk The chunk to delete
 */
void free_chunk(chunk *chunk);