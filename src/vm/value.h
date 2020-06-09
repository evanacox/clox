#pragma once

#include "../common/common.h"

typedef double value;

/**
 * Represents a list of constant values
 */
typedef struct value_array {
    /** The number of values in the array */
    int size;

    /** The max number of values the current array can hold */
    int capacity;

    /** Pointer to the first value */
    value *values;
} value_array;

/**
 * Initializes a value_array correctly
 * @param val_array The value array to initialize
 */
void init_value_array(value_array *val_array);

/**
 * Initializes a value_array with an initial size
 * @param val_array The value_array to initialize
 * @param capacity The initial size for the byte array
 */
void init_value_array_with_size(value_array *val_array, size_t capacity);

/**
 * Writes a value to the value array
 * @param val_array
 * @param val
 */
void write_value_array(value_array *val_array, value val);

/**
 * Frees a value array
 * @param val_array
 */
void free_value_array(value_array *val_array);

/**
 * Displays a value to stdout
 * @param val The value to display
 */
void print_value(value val);