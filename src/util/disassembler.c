#include "disassembler.h"
#include "../common/memory.h"
#include <stdio.h>

/**
 * Gets the line that an offset is on
 * @param lines Pointer to the line array
 * @param offset The offset to look for
 * @return The line the offset is on
 */
static inline size_t get_line(size_t *lines, int offset) {
    size_t idx = 0;

    // array is lined up as: [ n of bytes with first line number, first line number, ... ]
    // Read each "number of bytes" and subtract offset by it (and jump forward) until offset
    // is smaller, at which point the current "number of bytes" is the offset's line number
    //
    // see: https://en.wikipedia.org/wiki/Run-length_encoding
    while (lines[idx] < offset + 1) {
        offset -= lines[idx];
        idx += 2;
    }

    return lines[idx + 1];
}

/**
 * Prints a "simple instruction"
 * @param name The name of the instruction
 * @param offset The offset the instruction is at
 * @return The offset for the next instruction
 */
static inline int simple_instruction(const char *name, int offset) {
    printf("%-20s\n", name);

    return offset + 1;
}

/**
 * Prints a LOAD instruction
 * @param name The name of the instruction
 * @param c Pointer to the chunk
 * @param offset The offset of the instruction
 * @return The offset of the next instruction
 */
static inline int const_instruction(const char *name, chunk *c, int offset) {
    printf("%-20s %4s %d ", name, "idx:", c->code[offset + 1]);
    print_value(c->constant_pool.values[c->code[offset + 1]]);
    printf("\n");

    return offset + 2;
}

/**
 * Prints a LOAD_LONG instruction
 * @param n The name of the instruction
 * @param c Pointer to the chunk
 * @param offset The offset of the instruction
 * @return The offset of the next instruction
 */
static inline int const_long_instruction(const char *n, chunk *c, int offset) {
    // get the bytes of the 3 byte const_pool offset, turn into a size_t
    // to use as the index
    size_t num = from_bytes(c->code[offset + 1], c->code[offset + 2], c->code[offset + 3]);

    printf("%-20s %4s %zu", n, "idx:", num);
    print_value(c->constant_pool.values[num]);
    printf("\n");

    return offset + 4;
}

void disassemble_chunk(chunk *c, const char *name) {
    printf("=== %s ===\n", name);
    int offset = 0;

    while (offset < c->size) {
        offset = disassemble_instruction(c, offset);
    }
}

int disassemble_instruction(chunk *c, int offset) {
    printf("%04d ", offset);

    size_t line = get_line(c->lines, offset);
    // since the AND is short-circuited, get_line(offset - 1) won't get called
    // if offset is 0
    if (offset > 0 && line == get_line(c->lines, offset - 1)) {
        printf("   | ");
    } else {
        printf("%4zu ", line);
    }

    switch (c->code[offset]) {
        case OP_RETURN: return simple_instruction("OP_RETURN", offset);
        case OP_LOAD_CONST: return const_instruction("OP_LOAD_CONST", c, offset);
        case OP_LOAD_CONST_LONG: return const_long_instruction("OP_LOAD_CONST_LONG", c, offset);
        case OP_NEGATE: return simple_instruction("OP_NEGATE", offset);
        case OP_ADD: return simple_instruction("OP_ADD", offset);
        // case OP_SUBTRACT: return simple_instruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY: return simple_instruction("OP_MULTIPLY", offset);
        case OP_DIVIDE: return simple_instruction("OP_DIVIDE", offset);
        case OP_NIL: return simple_instruction("OP_NIL", offset);
        case OP_TRUE: return simple_instruction("OP_TRUE", offset);
        case OP_FALSE: return simple_instruction("OP_FALSE", offset);
        case OP_NOT: return simple_instruction("OP_NOT", offset);
        case OP_EQUAL: return simple_instruction("OP_EQUAL", offset);
        case OP_GREATER: return simple_instruction("OP_GREATER", offset);
        case OP_LESS: return simple_instruction("OP_LESS", offset);
        default: printf("Unknown opcode: %d\n", c->code[offset]); return offset + 1;
    }
}