#ifndef DAEMON_H
#define DAEMON_H

#include "watcher.h"

typedef struct {
    int fd;
    int wd;
    String dirname;
    String backup;
    // Vector watchers; // Vector<Watcher>
    Vector edits;
} Daemon;

int Daemon_watch(Daemon* w, uint32_t mask) {
    Vector dirs;
    Vector_init(dirs);
    
    String name;
    String_init(name);

    String_copy(name, w->dirname);

    int fallback = 1;
    int i = 0;
    while(i >= 0) {
        if(fallback) {
            CHECK_NULL(*Vector_access(DIR*, dirs, i) = opendir(name.data));
        }
        fallback = 0;

        struct dirent* e;
        while((e = readdir(Vector_data(DIR*, dirs)[i])) != NULL) {
            if (strcmp(e->d_name, ".") == 0
             || strcmp(e->d_name, "..") == 0)
                continue;

            Path_join(name, e->d_name);

            struct stat s;
            CHECK(stat(name.data, &s));
            if(S_ISDIR(s.st_mode)) {
                ++i;
                fallback = 1;
                break;
            }
            else if(S_ISREG(s.st_mode) && is_text_file(name.data)) {
                int wd;
                CHECK(wd = inotify_add_watch(w->fd, name.data, mask));
                //CHECK(copy_file(backup, name));
                // Watcher_init(Vector_access(Watcher, w->watchers, wd - 1), &name);
            }

            Path_go_up(name);
        }

        if(!fallback) {
            CHECK(closedir(Vector_data(DIR*, dirs)[i]));
            --i;
            Path_go_up(name);
        }
    }

    String_free(name);
    return 0;
}

int Daemon_init(Daemon* o, char* dirname, char* backup) {
    CHECK(o->fd = inotify_init());

    String_init(o->dirname);
    String_init(o->backup);

    Path_assign(o->dirname, dirname);
    Path_assign(o->backup, backup);

    // Vector_init(o->watchers);
    Vector_init(o->edits);
    return 0;
}

int Daemon_process_event(Daemon* o, struct inotify_event* e) {
    if (e->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (e->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (e->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (e->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (e->mask & IN_CREATE)        printf("IN_CREATE ");
    if (e->mask & IN_DELETE)        printf("IN_DELETE ");
    if (e->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (e->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (e->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (e->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (e->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (e->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (e->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (e->mask & IN_OPEN)          printf("IN_OPEN ");
    if (e->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (e->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    puts("");
    if(e->len)
        puts(e->name);
    // Watcher_diff(&Vector_data(Watcher, o->watchers)[e->wd - 1], &o->edits);
    return 0;
}

int Daemon_dump_edits(Daemon* o, int fd) {
    Vector_foreach(WatcherEdit, o->edits, e) {
        CHECK(WatcherEdit_dump(e, fd));
    }
    return 0;
}

int Daemon_wait(Daemon* o) {
    // Vector_foreach(Watcher, o->watchers, w) {
    //     Watcher_start(w);
    // }
    CHECK(o->wd = inotify_add_watch(o->fd, o->dirname.data, IN_ALL_EVENTS));

    struct inotify_event* e = (struct inotify_event*)malloc(sizeof(struct inotify_event) + PATH_MAX);
    
    ssize_t n;
    while((n = read(o->fd, e, sizeof(struct inotify_event))) > 0) {
        printf("%u\n", e->len);
        // if(e->len != 0)
        //     CHECK(n = read(o->fd, e->name, e->len));
        Daemon_process_event(o, e);
    }
    CHECK(n);

    free(e);
    return 0;
}

int Daemon_free(Daemon* o) {
    close(o->fd);
    String_free(o->dirname);
    String_free(o->backup);
    // Vector_free_f(Watcher, o->watchers, Watcher_free);
    Vector_free_f(Watcher, o->edits, Watcher_free);
    return 0;
}

#endif // !DAEMON_H
