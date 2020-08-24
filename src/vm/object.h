#pragma once

#include "../common/common.h"
#include "value.h"
#include <assert.h>

/**
 * Represents the type of the object
 */
typedef enum obj_type { OBJ_STRING, OBJ_FUNCTION, OBJ_INSTANCE } obj_type;

/**
 * Represents the "state" of the object, for bookkeeping purposes
 */
typedef struct object {
    /** Tag for the other object types */
    obj_type type;

    /** Pointer to the next object */
    struct object *next;
} object;

/**
 * Represents a string object
 */
typedef struct string {
    /** Holds the type / other bookkeeping information */
    object header;

    /** The number of characters in the string */
    int len;

    /** Pointer to the first character of the string */
    char *chars;
} string;

/**
 * Returns whether an object is of type `type`
 * @param val The value to check
 * @param type The object type
 * @return Whether the object is of type `type`
 */
static inline bool is_obj_type(value val, obj_type type) {
    return is_object(val) && as_object(val)->type == type;
}

/**
 * Returns if an object is a String
 * @param val The value to check
 * @return If the value is both an object and a string
 */
static inline bool is_string(value val) {
    return is_obj_type(val, OBJ_STRING);
}

/**
 * Reinterprets a value as a String object
 * @param val The value holding the string
 * @return The string object contained in the value
 */
static inline string *as_string(value val) {
    assert(is_string(val) && "Value being coerced to a string must be a string");

    return (string *)as_object(val);
}

/**
 * Reinterprets a value as a String object and returns the C string contained in that
 * @param val The value holding the string
 * @return The char pointer inside the String object
 */
static inline char *as_c_string(value val) {
    assert(is_string(val) && "Value being coerced to a string must be a string");

    return as_string(val)->chars;
}

/**
 * Copies a C string into a String object
 * @param chars Pointer to the first character
 * @param length The number of characters to copy
 * @return Pointer to the new string object
 */
string *copy_string(const char *chars, int length);

/**
 * Consumes a C string into a String object
 * @param chars The C string to use for the new String object
 * @param length The length of the string
 * @return Pointer to the new String object
 */
string *from_string(char *chars, int length);

/**
 * Prints an object
 * @param obj_val The value holding the object to print
 */
void print_object(value obj_val);