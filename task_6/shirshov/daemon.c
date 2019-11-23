#include "files.h"
#include "watcher.h"
#include "diff.h"

/*
./daemon dir1

./daemon 
*/

int main(int argc, char** argv) {
    // Daemon w;
    // CHECK(Daemon_init(&w, "dir1", "backup"));
    // CHECK(Daemon_watch(&w,
    //     IN_MODIFY
    // //   | IN_ALL_EVENTS
    // ));
    // CHECK(Daemon_wait(&w));
    // CHECK(Daemon_free(&w));

    String s1, s2;
    String_init(s1); String_init(s2);
    String_assign(s1, "This is cat!"); String_assign(s2, "That is dog!");
    
    Vec t = diff(s1, s2);
    Vec_foreach(Edit, t, v) {
        printf("Replace %lu %lu with %lu %lu", v->x_start, v->x_end, v->y_start, v->y_end);
        puts("");
    }

    Vec_free(t);

    String_free(s1); String_free(s2);
    return 0;
}
