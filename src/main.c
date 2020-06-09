#include "common/common.h"
#include "common/memory.h"
#include "util/disassembler.h"
#include "vm/chunk.h"
#include "vm/vm.h"

int main(int argc, const char **argv) {
    init_vm();

    chunk chunk;

    init_chunk(&chunk);
    size_t idx = add_constant(&chunk, 6.9);

    // write_byte(&chunk, OP_RETURN, 1);
    write_byte(&chunk, OP_LOAD_CONST, 2);
    write_byte(&chunk, idx, 2);

    size_t other_idx = add_constant(&chunk, 5.42);
    write_byte(&chunk, OP_LOAD_CONST_LONG, 3);
    uint8_t bytes[3];
    get_bytes(bytes, other_idx);
    write_byte(&chunk, bytes[0], 3);
    write_byte(&chunk, bytes[1], 3);
    write_byte(&chunk, bytes[2], 3);
    // write_constant(&chunk, 5.5, 3);

    disassemble_chunk(&chunk, "example chunk");
    interpret(&chunk);

    free_chunk(&chunk);

    free_vm();

    return 0;
}
