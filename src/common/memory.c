#include "memory.h"
#include <stdlib.h>

void *reallocate(void *old, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(old);
        return NULL;
    }

    return realloc(old, new_size);
}

void free_object(object *ptr) {
    switch (ptr->type) {
        case OBJ_STRING: {
            string *str = (string *)ptr;
            FREE_ARRAY(str->chars, char, str->len + 1);
            FREE(string, str);
            break;
        }
    }
}