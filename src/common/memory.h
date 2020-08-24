#pragma once

#include "../vm/object.h"
#include "common.h"

/**
 * Returns the new capacity for an chunk
 * @param old_capacity The old capacity for the chunk
 * @return The new capacity
 */
static inline size_t grow_capacity(size_t old_capacity) {
    if (old_capacity < 8) { return 8; }

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
 * @return A pointer to the array
 */
#define GROW_ARRAY(ptr, type, old_count, new_count)                                                \
    reallocate(ptr, sizeof(type) * (old_count), sizeof(type) * (new_count))

/**
 * Frees an array
 * @param ptr The array to free
 * @param type The type of the array (for use with sizeof)
 * @param count The capacity of the array
 * @return A pointer to the array
 */
#define FREE_ARRAY(ptr, type, count) reallocate(ptr, sizeof(type) * (count), 0)

/**
 * Allocates an array of `type` with `length` elements
 * @param type The type of the array
 * @param length The length of the array
 * @return A pointer to the array
 */
#define ALLOCATE(type, length) reallocate(NULL, 0, sizeof(type) * (length))

/**
 * Frees an object
 * @param type Type of the object
 * @param ptr Pointer to the object
 */
#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0)

/**
 * Reallocates a block of memory to be a new size
 *
 * If `old_size` is 0, a new block is allocated. If `new_size` is 0, the block
 * is freed. If `old_size` is larger than `new_size`, the block is shrunken.
 * Otherwise, the block is grown.
 *
 * @param old Pointer to the old block of memory
 * @param old_size Size of the block
 * @param new_size The new size of the block
 * @return A pointer to the block
 */
void *reallocate(void *old, size_t old_size, size_t new_size);

/**
 * Frees an object
 * @param ptr Pointer to the object
 */
void free_object(object *ptr);
