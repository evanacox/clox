#pragma once

#include "../vm/chunk.h"

/**
 * Disassembles and prints an chunk
 * @param chunk The chunk to view
 * @param name The name to print the chunk with
 */
void disassemble_chunk(chunk *chunk, const char *name);

/**
 * Prints a human-readable version of the instruction
 * @param chunk The chunk the instruction is in
 * @param offset The offset of the instruction in its chunk
 * @param instruction The byte representing the instruction
 * @returns The number of bytes consumed by the instruction
 */
int disassemble_instruction(chunk *chunk, int offset);
