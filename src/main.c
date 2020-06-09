#include "common/common.h"
#include "common/memory.h"
#include "util/disassembler.h"
#include "vm/chunk.h"

int main(int argc, const char **argv) {
    chunk chunk;

    init_chunk(&chunk);
    size_t idx = add_constant(&chunk, 6.9);

    write_byte(&chunk, OP_RETURN, 1);
    write_byte(&chunk, OP_LOAD_CONST, 2);
    write_byte(&chunk, idx, 2);

    size_t_bytes n;
    n.number = add_constant(&chunk, 5.5);
    write_byte(&chunk, OP_LOAD_CONST_LONG, 3);
    write_byte(&chunk, n.raw.one, 3);
    write_byte(&chunk, n.raw.two, 3);
    write_byte(&chunk, n.raw.three, 3);

    disassemble_chunk(&chunk, "example chunk");
    free_chunk(&chunk);

    return 0;
}
