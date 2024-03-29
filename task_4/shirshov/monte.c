#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define PI 3.14159265358979323846

typedef struct {
    float x, y;
} Point;

typedef int (*Predicate)(Point, void*);

typedef struct {
    Predicate f;
    void* pred_info;

    Point left_down;
    Point right_up;
    Point size;

    size_t n;
    float s;
} FunctionInfo;

float frand(unsigned* seed) {
    return (float)rand_r(seed) / (float)INT_MAX;
}

Point fi_next_random(FunctionInfo* info, unsigned* seed) {
    Point res;
    res.x = info->left_down.x + info->size.x * frand(seed);
    res.y = info->left_down.y + info->size.y * frand(seed);
    return res;
}

int fi_check(FunctionInfo* info, Point p) {
    return (*info->f)(p, info->pred_info);
}

void* routine(void* arg) {
    FunctionInfo* info = (FunctionInfo*)arg;
    unsigned seed = time(NULL);
    
    size_t i, res = 0;
    for(i = 0; i < info->n; ++i) {
        Point p = fi_next_random(info, &seed);
        if(fi_check(info, p)) {
            ++res;
        }
    }

    pthread_exit((void*)res);
}

typedef struct {
    Point o;
    float r2;
    float s;
} CircleInfo;

int circle(Point p, void* ptr) {
    CircleInfo* info = (CircleInfo*)ptr;
    p.x -= info->o.x;
    p.y -= info->o.y;
    float r2 = p.x * p.x + p.y * p.y;
    return r2 < info->r2;
}

int main(int argc, char** argv) {
    size_t max_number_of_threads = (size_t)atoi(argv[1]);
    size_t number_of_dots = (size_t)atoi(argv[2]);

    CircleInfo c;
    c.o.x = 0;
    c.o.y = 0;
    c.r2 = 25;
    c.s = PI * c.r2;

    FunctionInfo info;
    info.f = circle;
    info.pred_info = &c;
    info.left_down.x = -5;
    info.left_down.y = -5;
    info.right_up.x = 5;
    info.right_up.y = 5;
    info.size.x = 10;
    info.size.y = 10;
    info.s = info.size.x * info.size.y;

    pthread_attr_t tattr;
    pthread_attr_init(&tattr); // default attributes

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * max_number_of_threads);

    size_t n;
    for(n = 1; n < max_number_of_threads; n *= 2) {
        //if(n == 11) n = 10;

        clock_t start = clock();
        info.n = number_of_dots / n;

        size_t i;
        for(i = 0; i < n; ++i) {
            pthread_create(threads + i, &tattr, routine, &info);
        }

        float r = 0;
        for(i = 0; i < n; ++i) {
            size_t t;
            pthread_join(threads[i], (void**)&t);
            r += (float)t / (float)info.n;
        }
        r *= info.s;
        r /= n;

        clock_t end = clock();
        float secs = (float)(end - start) / (float)CLOCKS_PER_SEC;

        printf(
            // "Real area: %f\n"
            // "Computed area: %f\n"
            "Number of threads: %lu\n"
            "Time %f sec\n\n",
            // c.s, r,
            n, secs
        );
    }

    return 0;
}
