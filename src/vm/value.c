#include "value.h"
#include "../common/memory.h"
#include "object.h"
#include <stdio.h>
#include <string.h>

void init_value_array(value_array *val_array) {
    val_array->size = 0;
    val_array->capacity = 0;
    val_array->values = NULL;
}

void init_value_array_with_capacity(value_array *val_array, size_t capacity) {
    val_array->size = 0;
    val_array->capacity = capacity;
    val_array->values = reallocate(NULL, 0, sizeof(value) * capacity);
}

void write_value_array(value_array *val_array, value val) {
    if (val_array->size + 1 > val_array->capacity) {
        int new_capacity = grow_capacity(val_array->capacity);
        val_array->values = GROW_ARRAY(val_array->values, value, val_array->capacity, new_capacity);
        val_array->capacity = new_capacity;
    }

    val_array->values[val_array->size++] = val;
}

void free_value_array(value_array *val_array) {
    FREE_ARRAY(val_array->values, value, val_array->capacity);
    init_value_array(val_array);
}

bool are_equal(value lhs, value rhs) {
    if (lhs.type != rhs.type) {
        // Lox doesn't implicitly convert types for comparisons
        return false;
    }

    // we can't just do a memcmp() or a `rhs.as.number == lhs.as.number`,
    // because C makes no guarantee about the unused bits being 0 (or anything, for that
    // matter). so, two equivalent values could return false seemingly randomly
    switch (lhs.type) {
        case VAL_BOOL: return as_bool(lhs) == as_bool(rhs);
        case VAL_NIL: return true;
        case VAL_NUMBER: return as_number(lhs) == as_number(rhs);
        case VAL_OBJ: {
            string *lh_str = as_string(lhs);
            string *rh_str = as_string(rhs);

            if (lh_str->len != rh_str->len) { return false; }

            return memcmp(lh_str->chars, rh_str->chars, lh_str->len) == 0;
        }
    }
}

void print_value(value val) {
    switch (val.type) {
        case VAL_NUMBER: printf("%g", as_number(val)); break;
        case VAL_BOOL: printf(as_bool(val) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_OBJ: print_object(val); break;
    }
}