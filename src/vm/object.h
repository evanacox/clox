#pragma once

#include "../common/common.h"
#include "value.h"

/**
 * Represents the type of the object
 */
typedef enum obj_type { OBJ_STRING, OBJ_FUNCTION, OBJ_INSTANCE } obj_type;

/**
 * Represents the "state" of the object, for bookkeeping purposes
 */
typedef struct object {
    obj_type type;
} object;

/**
 * Represents a string object
 */
typedef struct string {
    object obj;
    unsigned int len;
    char *chars;
} string;