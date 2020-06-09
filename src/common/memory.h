#pragma once

#include "common.h"

/**
 * Returns the new capacity for an chunk
 * @param old_capacity The old capacity for the chunk
 * @return The new capacity
 */
static inline size_t grow_capacity(size_t old_capacity) {
    if (old_capacity < 8) {
        return 8;
    }

    return old_capacity * 2;
}

/**
 * Creates a size_t from three bytes
 * @param one The first byte
 * @param two The second byte
 * @param three The third byte
 * @return The bytes as a normal number
 */
static inline size_t from_bytes(uint8_t one, uint8_t two, uint8_t three) {
    return one + (two << 8u) + (three << 16u);
}

/**
 * Gets three bytes from a size_t
 * @param bytes A pre-allocated buffer with space for at least 3 bytes
 * @param n The number to turn into bytes
 */
static inline void get_bytes(uint8_t *bytes, size_t n) {
    bytes[0] = n & 0xFFu;
    bytes[1] = (n & 0xFF00u) >> 8u;
    bytes[2] = (n & 0xFF0000u) >> 16u;
}

/**
 * Expands an array
 * @param ptr The array to expand
 * @param type The type of the array (for use with sizeof)
 * @param old_count The old capacity of the array
 * @param new_count The new capacity of the array
 */
#define GROW_ARRAY(ptr, type, old_count, new_count)                            \
    reallocate(ptr, sizeof(type) * (old_count), sizeof(type) * (new_count))

/**
 * Frees an array
 * @param ptr The array to free
 * @param type The type of the array (for use with sizeof)
 * @param count The capacity of the array
 */
#define FREE_ARRAY(ptr, type, count) reallocate(ptr, sizeof(type) * (count), 0)

/**
 * Reallocates a byte array to be a new size
 *
 * If `old_size` is 0, a new block is allocated. If `new_size` is 0, the block
 * is freed. If `old_size` is larger than `new_size`, the array is shrunken.
 * Otherwise, the array is grown.
 *
 * @param old Pointer to the first element in the array
 * @param old_size Size of the array
 * @param new_size The new size of the array
 * @return A pointer to the first element of the reallocated array
 */
void *reallocate(void *old, size_t old_size, size_t new_size);
