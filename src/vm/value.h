#pragma once

#include "../common/common.h"
#include "object.h"

/** The type of a Lox value */
typedef enum value_type { VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ } value_type;

typedef struct value {
    value_type type;

    union {
        bool boolean;
        double number;
        void *obj;
    } as;
} value;

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

/**
 * Returns a value struct with a boolean value
 * @param val The boolean value
 * @return The value struct with a bool
 */
static inline value bool_value(bool val) {
    return ((value){VAL_BOOL, {.boolean = val}});
}

/**
 * Returns a value struct with a double value
 * @param val The numeric value
 * @return The value struct with a number
 */
static inline value number_value(double val) {
    return ((value){VAL_NUMBER, {.number = val}});
}

/**
 * Returns a value struct with a value of nil
 * @return The value struct with nil
 */
static inline value nil_value() {
    return ((value){VAL_NIL, {.boolean = false}});
}

/**
 * Returns if a value is a bool
 * @param val The value to check
 * @return If the value is of type VAL_BOOL
 */
static inline bool is_bool(value val) {
    return val.type == VAL_BOOL;
}

/**
 * Returns if a value is nil
 * @param val The value to check
 * @return Whether or not the value is of type VAL_NIL
 */
static inline bool is_nil(value val) {
    return val.type == VAL_NIL;
}

/**
 * Returns if a value is a number
 * @param val The value to check
 * @return Whether or not the value is of type VAL_NUMBER
 */
static inline bool is_number(value val) {
    return val.type == VAL_NUMBER;
}

/**
 * Returns the bool value of a value struct
 * @param val The value struct
 * @return The boolean value
 */
static inline bool as_bool(value val) {
    return val.as.boolean;
}

/**
 * Returns the double value of a value struct
 * @param val The value struct
 * @return The double value
 */
static inline double as_number(value val) {
    return val.as.number;
}

/**
 * Checks if two values are equal
 * @param rhs The right hand side of the ==
 * @param lhs The left hand side of the ==
 * @return Whether the values are equal
 */
static inline bool are_equal(value lhs, value rhs) {
    if (lhs.type != rhs.type) {
        // Lox doesn't implicitly convert types for comparisons
        return false;
    }

    // we can't just do a memcmp() or a `rhs.as.number == lhs.as.number`,
    // because C makes no guarantee about the unused bits being 0 (or anything, for that matter).
    // so, two equivalent values could return false seemingly randomly
    switch (lhs.type) {
        case VAL_BOOL: return as_bool(lhs) == as_bool(rhs);
        case VAL_NIL: return true;
        case VAL_NUMBER: return as_number(lhs) == as_number(rhs);
    }
}