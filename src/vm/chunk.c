#include "chunk.h"
#include "../common/memory.h"
#include "value.h"

static inline void write_line(chunk *c, size_t line) {
    // clang-format off
    if (
        c->lines_size != 0 &&
        c->lines[c->lines_size - 1] == line)
    // clang-format on
    {
        c->lines[c->lines_size - 2] += 1;
        return;
    }

    if (c->lines_size + 2 > c->lines_capacity) {
        int new_cap = grow_capacity(c->lines_capacity);
        c->lines = GROW_ARRAY(c->lines, size_t, c->lines_capacity, new_cap);
        c->lines[c->lines_size] = 0;
        c->lines_capacity = new_cap;
    }

    c->lines_size += 2;
    c->lines[c->lines_size - 2] += 1;
    c->lines[c->lines_size - 1] = line;
}

void init_chunk_with_size(chunk *c, size_t capacity) {
    c->capacity = capacity;
    c->size = 0;
    c->code = GROW_ARRAY(NULL, uint8_t, 0, capacity);
    c->lines = NULL;
    c->lines_capacity = 0;
    c->lines_size = 0;

    init_value_array(&c->constant_pool);
}

void init_chunk(chunk *c) {
    c->capacity = 0;
    c->size = 0;
    c->code = NULL;
    c->lines = NULL;
    c->lines_capacity = 0;
    c->lines_size = 0;

    init_value_array(&c->constant_pool);
}

void write_byte(chunk *c, uint8_t byte, size_t line) {
    if (c->size + 1 > c->capacity) {
        int new_capacity = grow_capacity(c->capacity);
        c->code = GROW_ARRAY(c->code, uint8_t, c->capacity, new_capacity);
        c->lines = GROW_ARRAY(c->lines, size_t, c->capacity, new_capacity);
        c->capacity = new_capacity;
    }

    c->code[c->size++] = byte;

    write_line(c, line);
}

void write_constant(chunk *c, value constant, size_t line) {
    size_t offset = add_constant(c, constant);

    if (offset <= UINT8_MAX) {
        write_byte(c, OP_LOAD_CONST, line);
        write_byte(c, offset, line);
    } else {
        uint8_t bytes[3];
        get_bytes(bytes, offset);

        write_byte(c, OP_LOAD_CONST_LONG, line);
        write_byte(c, bytes[0], line);
        write_byte(c, bytes[1], line);
        write_byte(c, bytes[2], line);
    }
}

int add_constant(chunk *c, value constant) {
    write_value_array(&c->constant_pool, constant);

    return c->constant_pool.size - 1;
}

void free_chunk(chunk *c) {
    FREE_ARRAY(c->code, uint8_t, c->capacity);
    FREE_ARRAY(c->lines, size_t, c->capacity);
    free_value_array(&c->constant_pool);
    init_chunk(c);
}