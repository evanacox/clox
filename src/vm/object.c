#include "object.h"
#include "../common/memory.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Allocates an object of `type`
 * @param type The object type to allocate
 * @param obj_type The type tag to assign it
 * @return A pointer to the object
 */
#define ALLOCATE_OBJECT(type, obj_type) (type *)allocate_object(sizeof(type), obj_type)

/**
 * Allocates an object
 * @param size The size of the object
 * @param type The type tag
 * @return A pointer to the object
 */
static inline object *allocate_object(size_t size, obj_type type) {
    object *obj = reallocate(NULL, 0, size);
    obj->type = type;
    obj->next = g_vm.objects;
    g_vm.objects = obj;

    return obj;
}

/**
 * Allocates a String object
 * @param chars Pointer to the C string
 * @param len The number of characters
 * @return A pointer to the string object
 */
static inline string *allocate_string(char *chars, int len) { //
    string *str = ALLOCATE_OBJECT(string, OBJ_STRING);
    str->len = len;
    str->chars = chars;

    return str;
}

string *copy_string(const char *chars, int len) {
    assert(strlen(chars) >= len && "String length should not exceed len param");

    char *heap_chars = ALLOCATE(char, len + 1);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';

    return allocate_string(heap_chars, len);
}

string *from_string(char *chars, int len) {
    assert(strlen(chars) == len && "from_string(chars, len) len should be correct");

    return allocate_string(chars, len);
}

void print_object(value obj_val) {
    switch (as_object(obj_val)->type) {
        case OBJ_STRING: printf("%s", as_c_string(obj_val)); break;
        case OBJ_INSTANCE:
        case OBJ_FUNCTION: exit(-1);
    }
}