cmake_minimum_required(VERSION 3.17)
project(clox C)

set(CMAKE_C_STANDARD 11)

set(HEADER_FILES src/common/common.h src/vm/chunk.h src/common/memory.h src/util/disassembler.h src/vm/value.h)
set(SOURCE_FILES src/main.c src/vm/chunk.c src/common/memory.c src/util/disassembler.c src/vm/value.c)

add_executable(clox ${HEADER_FILES} ${SOURCE_FILES})