#include "daemon.h"

int main(int argc, char** argv) {
#if 1
    Daemon w;
    CHECK(Daemon_init(&w, "dir1", "backup"));
    // CHECK(Daemon_watch(&w,
    //     IN_MODIFY
    // //   | IN_ALL_EVENTS
    // ));
    CHECK(Daemon_wait(&w));
    // sleep(10);
    CHECK(Daemon_dump_edits(&w, STDOUT_FILENO));
    CHECK(Daemon_free(&w));
#else
    String s1, s2;
    String_init(s1); String_init(s2);
    String_assign(s1, "Hello, world!"); String_assign(s2, "Hell, work?");

    Vector res = diff(s1, s2);
    Vector_foreach(Edit, res, e) {
        printf("Replace %lu %lu with %lu %lu\n", e->x_start, e->x_end, e->y_start, e->y_end);
    }
    Vector_free(res);

    res = diff(s2, s1);
    Vector_foreach(Edit, res, e) {
        printf("Replace %lu %lu with %lu %lu\n", e->x_start, e->x_end, e->y_start, e->y_end);
    }
    Vector_free(res);

    String_free(s1); String_free(s2);
#endif
    return 0;
}
