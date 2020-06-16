#pragma once

#include "../vm/chunk.h"

/**
 * Compiles the source code into bytecode
 * @param source The Lox source code
 * @param chunk The chunk to write data to
 * @return Whether or not the compilation was successful
 */
bool compile(const char *source, chunk *chunk);