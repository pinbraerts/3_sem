#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} Vec, String;

void pVec_init(Vec* o) {
    o->data = NULL;
    o->size = 0;
    o->capacity = 0;
}
#define Vec_init(vec) pVec_init(&vec)

void __Vec_resize(Vec* o, size_t size, size_t element_size) {
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
#define pVec_resize(T, o, s) __Vec_resize(o, s, sizeof(T))
#define Vec_resize(T, o, s) pVec_resize(T, (&o), s)

void pVec_free(Vec* o) {
    free(o->data);

    o->data = NULL;
    o->size = 0;
    o->capacity = 0;
}
#define Vec_free(vec) pVec_free(&vec)

void* __Vec_back_new(Vec* o, size_t element_size) {
    __Vec_resize(o, o->size + 1, element_size);
    return (o->data + element_size * (o->size - 1));
}
#define pVec_back_new(T, o) ((T*)__Vec_back_new(o, sizeof(T)))
#define Vec_back_new(T, o) pVec_back_new(T, (&o))

void* __Vec_access(Vec* o, size_t element_size, size_t index) {
    if(index >= o->size)
        __Vec_resize(o, index + 1, element_size);
    return (o->data + element_size * index);
}
#define pVec_access(T, o, i) ((T*)__Vec_access(o, sizeof(T), (i)))
#define Vec_access(T, o, i) pVec_access(T, (&o), i)

void __Vec_copy(Vec* dest, const Vec* src, size_t element_size) {
    __Vec_resize(dest, src->size, element_size);
    memcpy(dest->data, src->data, src->size * element_size);
}
#define pVec_copy(T, dest, src) __Vec_copy(dest, src, sizeof(T))
#define Vec_copy(T, dest, src) pVec_copy(T, &dest, &src)

void pVec_move(Vec* dest, Vec* src) {
    dest->data = src->data;
    dest->size = src->size;
    dest->capacity = src->capacity;
    
    pVec_init(src);
}
#define Vec_move(dest, src) pVec_move(&dest, &src)

#define pVec_data(T, o) ((T*)((o)->data))
#define Vec_data(T, o) pVec_data(T, &o)

#define pVec_foreach(T, o, p) for(T *p = pVec_data(T, o), *__Vec_end = p + o->size; p < __Vec_end; ++p)
#define Vec_foreach(T, o, p) pVec_foreach(T, (&o), p)

#define pVec_fill(T, o, x) do { T f = (x); pVec_foreach(T, o, y) { *y = f; } } while(0)
#define Vec_fill(T, o, x) pVec_fill(T, (&o), x)

#define pVec_free_f(T, o, f) do { pVec_foreach(T, o, x) f(x); pVec_free(o); } while(0)
#define Vec_free_f(T, o, f) pVec_free_f(T, (&o), f)

#define String_init(o) Vec_init(o)
#define String_free(o) Vec_free(o)

void pString_resize(String* o, size_t size) {
    if(size > 0) {
        __Vec_resize(o, size + 1, sizeof(char));
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

#define String_move(dest, src) Vec_move(dest, src)

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

#endif // !VECTOR_H
