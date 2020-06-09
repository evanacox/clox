#include "value.h"
#include "../common/memory.h"
#include <stdio.h>

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
        val_array->values = GROW_ARRAY(
            val_array->values, value, val_array->capacity, new_capacity);
        val_array->capacity = new_capacity;
    }

    val_array->values[val_array->size++] = val;
}

void free_value_array(value_array *val_array) {
    FREE_ARRAY(val_array->values, value, val_array->capacity);
    init_value_array(val_array);
}

void print_value(value val) { printf("(Value: %g)\n", val); }