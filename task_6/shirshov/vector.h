#ifndef VectorTOR_H
#define VectorTOR_H

#include <stdlib.h>
#include <string.h>

#include "files.h"

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} Vector, String;

void pVector_init(Vector* o) {
    o->data = NULL;
    o->size = 0;
    o->capacity = 0;
}
#define Vector_init(Vector) pVector_init(&Vector)

void __Vector_resize(Vector* o, size_t size, size_t element_size) {
    size_t new_capacity = size * element_size;
    o->size = size;

    if(new_capacity <= o->capacity) {
        return;
    }

    o->capacity = new_capacity * 1.5;
    char* new_buf = (char*)realloc(o->data, o->capacity);
    if(new_buf == NULL) {
        puts("realloc() failed");
        free(o->data);
        o->data = NULL;
        return;
    }
    o->data = new_buf;
}
#define pVector_resize(T, o, s) __Vector_resize(o, s, sizeof(T))
#define Vector_resize(T, o, s) pVector_resize(T, (&o), s)

void pVector_free(Vector* o) {
    free(o->data);

    o->data = NULL;
    o->size = 0;
    o->capacity = 0;
}
#define Vector_free(Vector) pVector_free(&Vector)

void* __Vector_back_new(Vector* o, size_t element_size) {
    __Vector_resize(o, o->size + 1, element_size);
    return (o->data + element_size * (o->size - 1));
}
#define pVector_back_new(T, o) ((T*)__Vector_back_new(o, sizeof(T)))
#define Vector_back_new(T, o) pVector_back_new(T, (&o))

void* __Vector_access(Vector* o, size_t element_size, size_t index) {
    if(index >= o->size)
        __Vector_resize(o, index + 1, element_size);
    return (o->data + element_size * index);
}
#define pVector_access(T, o, i) ((T*)__Vector_access(o, sizeof(T), (i)))
#define Vector_access(T, o, i) pVector_access(T, (&o), i)

void __Vector_copy(Vector* dest, const Vector* src, size_t element_size) {
    __Vector_resize(dest, src->size, element_size);
    memcpy(dest->data, src->data, src->size * element_size);
}
#define pVector_copy(T, dest, src) __Vector_copy(dest, src, sizeof(T))
#define Vector_copy(T, dest, src) pVector_copy(T, &dest, &src)

void pVector_move(Vector* dest, Vector* src) {
    dest->data = src->data;
    dest->size = src->size;
    dest->capacity = src->capacity;
    
    pVector_init(src);
}
#define Vector_move(dest, src) pVector_move(&dest, &src)

#define pVector_data(T, o) ((T*)((o)->data))
#define Vector_data(T, o) pVector_data(T, &o)

#define pVector_foreach(T, o, p) for(T *p = pVector_data(T, o), *__Vector_end = p + o->size; p < __Vector_end; ++p)
#define Vector_foreach(T, o, p) pVector_foreach(T, (&o), p)

#define pVector_fill(T, o, x) do { T f = (x); pVector_foreach(T, o, y) { *y = f; } } while(0)
#define Vector_fill(T, o, x) pVector_fill(T, (&o), x)

#define pVector_free_f(T, o, f) do { pVector_foreach(T, o, x) f(x); pVector_free(o); } while(0)
#define Vector_free_f(T, o, f) pVector_free_f(T, (&o), f)

#define String_init(o) Vector_init(o)
#define String_free(o) Vector_free(o)

void pString_resize(String* o, size_t size) {
    if(size > 0) {
        __Vector_resize(o, size + 1, sizeof(char));
        o->data[--o->size] = '\0';
    }
    else {
        o->size = 0;
        if(o->capacity > 0)
            o->data[0] = '\0';
    }
}
#define String_resize(o, s) pString_resize((&o), s)

void pString_copy(String* dest, const String* src) {
    pString_resize(dest, src->size);
    strncpy(dest->data, src->data, src->size);
}
#define String_copy(dest, src) pString_copy(&dest, &src)

#define String_move(dest, src) Vector_move(dest, src)

void pString_append(String* o, const char* str) {
    size_t n = strlen(str);
    size_t sz = o->size;
    pString_resize(o, n + sz);
    strncpy(o->data + sz, str, n);
}
#define String_append(o, s) pString_append((&o), s)

void pString_char(String* o, char c) {
    pString_resize(o, o->size + 1);
    o->data[o->size - 1] = c;
}
#define String_char(o, c) pString_char((&o), c)

void pString_assign(String* o, const char* str) {
    pString_resize(o, strlen(str));
    strncpy(o->data, str, o->size);
}
#define String_assign(o, s) pString_assign((&o), s)

void pString_assign_n(String* o, const char* str, size_t n) {
    pString_resize(o, n);
    strncpy(o->data, str, n);
    o->data[o->size] = '\0';
}
#define String_assign_n(o, s, n) pString_assign_n((&o), s, n)

int pString_dump(const String* o, int fd) {
    CHECK(write_all(fd, &o->size, sizeof(size_t)));
    if(o->size > 0)
        CHECK(write_all(fd, o->data, o->size * sizeof(char)));
    return 0;
}
#define String_dump(o, f) pString_dump((&o), f)

int pString_load(String* o, int fd) {
    size_t s;
    CHECK(read_all(fd, &s, sizeof(size_t)));
    
    pString_resize(o, s);
    if(s > 0) {
        CHECK(read_all(fd, o->data, sizeof(char) * o->size));
        o->data[o->size] = '\0';
    }
    return 0;
}
#define String_load(o, f) pString_load((&o), f)

void pPath_trim(String* o) {
    if(o->data[o->size - 1] == '/')
        o->data[--o->size] = '\0';
}
#define Path_trim(o) pPath_trim((&o))

void pPath_go_up(String* o) {
    int i;
    for(i = o->size - 1; i >= 0; --i) {
        if(o->data[i] == '/') {
            break;
        }
    }
    if(i < 0) i = 0;
    pString_resize(o, i);
}
#define Path_go_up(o) pPath_go_up((&o))

#define Path_assign(o, s) do { String_assign(o, s); Path_trim(o); } while(0)
#define Path_join(o, s) do { String_char(o, '/'); String_append(o, s); Path_trim(o); } while(0)

#endif // !VectorTOR_H
