#ifndef DIFF_H
#define DIFF_H

#include "vector.h"

#include <stdio.h>

int max(int x, int y) {
    return x > y ? x : y;
}

int ind(int x, int mod) {
    if(x < 0)
        return mod + x;
    else
        return x;
}

typedef struct {
    size_t x_start, y_start, x_end, y_end;
} Edit;

Vector trace(String a, String b) {
    size_t m = a.size + b.size;
    Vector res; // <int>
    Vector t;

    Vector_init(t);
    Vector_init(res);

    Vector_resize(size_t, res, m + 2);
    Vector_fill(size_t, res, 0);
    // Vector_data(size_t, res)[1] = 0;

    int offset = res.size / 2 + (res.size % 2 != 0);

    for(int d = 0; d < offset; ++d) {
        for(int k = -(d - 2 * max(0, d - b.size)); k <= d - 2 * max(0, d - a.size); k += 2) {
            size_t minus = Vector_data(size_t, res)[k - 1 + offset];
            size_t plus = Vector_data(size_t, res)[k + 1 + offset];
            size_t x = ((k == -d) || (k != d && minus < plus)) ? plus : minus + 1;
            size_t y = x - k;
            for(; x < a.size && y < b.size && a.data[x] == b.data[y]; ++x, ++y);

            Vector_data(size_t, res)[k + offset] = x;
            // printf("%2d %lu %lu\n", k, x, y);

            if(x == a.size && y == b.size) {
                d = m + 1;
                break; // break outer
            }
        }

        Vector* v = Vector_back_new(Vector, t);
        pVector_init(v);
        pVector_copy(size_t, v, &res);
    }

    Vector_free(res);
    return t;
}

Vector diff(String a, String b) {
    Vector t = trace(a, b);

    Vector res; // <Edit>
    Vector_init(res);

    size_t x = a.size, y = b.size;

    for(int d = t.size - 1; d >= 0 && (x > 0 || y > 0); --d) {
        Vector* v = &Vector_data(Vector, t)[d];

        // printf("Line %d: ", d);
        // pVector_foreach(size_t, v, x) {
        //     printf("%lu ", *x);
        // }
        // puts("");

        int offset = v->size / 2 + (v->size % 2 != 0);

        int k = x - y;
        size_t x_end = pVector_data(size_t, v)[offset + k];
        size_t y_end = x_end - k;

        size_t minus = pVector_data(size_t, v)[k - 1 + offset];
        size_t plus = pVector_data(size_t, v)[k + 1 + offset];

        int down = (k == -d || (k != d && minus < plus));

        int prev_k = down ? k + 1 : k - 1;
        size_t x_start = pVector_data(size_t, v)[prev_k + offset];
        size_t y_start = x_start - prev_k;

        size_t x_mid = down ? x_start : x_start + 1;
        size_t y_mid = x_mid - k;

        // printf("%2d %2d %2d %lu %lu %lu %lu %lu %lu\n", k, prev_k, offset, x_start, x_mid, x_end, y_start, y_mid, y_end);

        if(x_mid == 0 && y_mid == 0)
            break;
        if(res.size == 0 || (x_end != x_mid || y_end != y_mid)) {
            // diagonal or first
            Edit* e = Vector_back_new(Edit, res);
            e->x_start = x_start;
            e->y_start = y_start;
            e->x_end = x_mid;
            e->y_end = y_mid;
        }
        else { // not diagonal
            Edit* e = &Vector_data(Edit, res)[res.size - 1];
            e->x_start = x_start;
            e->y_start = y_start;
        }

        x = x_start;
        y = y_start;
    }

    Vector_free_f(Vector, t, pVector_free);
    return res;
}

#endif // !DIFF_H
