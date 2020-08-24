#pragma once

#include "../common/common.h"
#include <assert.h>

/** The type of a Lox value */
typedef enum value_type { VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ } value_type;

/** Forward declaration for `object` */
typedef struct object object;

/**
 * Encapsulates a Lox value
 */
typedef struct value {
    /** Tag for the variant */
    value_type type;

    /** Variant holding the actual Lox val */
    union {
        bool boolean;
        double number;
        object *obj;
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
 * Creates an object from a pointer to the object
 * @param ptr Pointer to the object
 * @return The value
 */
static inline value object_value(object *ptr) {
    return ((value){VAL_OBJ, {.obj = ptr}});
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
 * Returns if a value is an object
 * @param val The value to check
 * @return Whether or not the value is of type VAL_OBJ
 */
static inline bool is_object(value val) {
    return val.type == VAL_OBJ;
}

/**
 * Returns the bool value of a value struct
 * @param val The value struct
 * @return The boolean value
 */
static inline bool as_bool(value val) {
    assert(is_bool(val) && "Value being coerced to a bool must be a bool");

    return val.as.boolean;
}

/**
 * Returns the double value of a value struct
 * @param val The value struct
 * @return The double value
 */
static inline double as_number(value val) {
    assert(is_number(val) && "Value being coerced to a number must be a number");

    return val.as.number;
}

/**
 * Returns the object pointer from a value
 * @param val The value
 * @return The object pointer
 */
static inline object *as_object(value val) {
    assert(is_object(val) && "Value being coerced to an object must be an object");

    return val.as.obj;
}

/**
 * Checks if two values are equal
 * @param rhs The right hand side of the ==
 * @param lhs The left hand side of the ==
 * @return Whether the values are equal
 */
bool are_equal(value lhs, value rhs);