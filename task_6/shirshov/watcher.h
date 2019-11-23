#ifndef DAEMON_H
#define DAEMON_H

#include "events.h"
#include "vector.h"
#include "diff.h"

typedef struct {
    String filename;
    String initial_buffer;
    String last_buffer;
    Vec edits; // Vec<Edit>
} Watcher;

void Watcher_init(Watcher* o, String* name) {
    String_init(o->filename);
    String_init(o->initial_buffer);
    String_init(o->last_buffer);
    pString_copy(&o->filename, name);
    Vec_init(o->edits);
}

int Watcher_start(Watcher* o) {
    int fd;
    CHECK(fd = open(o->filename.data, O_WRONLY));

    struct stat s;
    CHECK(stat(o->filename.data, &s));
    String_resize(o->initial_buffer, s.st_size);
    CHECK(read_all(fd, o->initial_buffer.data, s.st_size));
    String_copy(o->last_buffer, o->initial_buffer);
    CHECK(close(fd));

    return 0;
}

void Watcher_free(Watcher* o) {
    String_free(o->filename);
    String_free(o->initial_buffer);
    String_free(o->last_buffer);
    Vec_free_f(Edit, o->edits, free);
}

void Watcher_diff(Watcher* o) {

}

typedef struct {
    int fd;
    String dirname;
    String backup;
    Vec watchers; // Vec<Watcher>
} Daemon;

int Daemon_watch(Daemon* w, uint32_t mask) {
    Vec dirs;
    Vec_init(dirs);
    
    String name;
    String_init(name);

    String_copy(name, w->dirname);

    int fallback = 1;
    int i = 0;
    while(i >= 0) {
        if(fallback) {
            CHECK_NULL(*Vec_access(DIR*, dirs, i) = opendir(name.data));
        }
        fallback = 0;

        struct dirent* e;
        while((e = readdir(Vec_data(DIR*, dirs)[i])) != NULL) {
            if(strcmp(e->d_name, ".") == 0
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
                Watcher_init(Vec_access(Watcher, w->watchers, wd - 1), &name);
            }

            Path_go_up(name);
        }

        if(!fallback) {
            CHECK(closedir(Vec_data(DIR*, dirs)[i]));
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

    Vec_init(o->watchers);
    return 0;
}

int Daemon_process_event(Daemon* w, struct inotify_event* e) {
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
    if(e->len)
        puts(e->name);
    puts(Vec_data(Watcher, w->watchers)[e->wd - 1].filename.data);
    return 0;
}

int Daemon_wait(Daemon* w) {
    struct inotify_event* e = (struct inotify_event*)malloc(sizeof(struct inotify_event) + 1024);
    
    ssize_t n;
    while((n = read(w->fd, e, sizeof(struct inotify_event))) > 0) {
        if(e->len != 0)
            CHECK(n = read(w->fd, e->name, e->len));
        Daemon_process_event(w, e);
    }
    CHECK(n);

    free(e);
    return 0;
}

int Daemon_free(Daemon* w) {
    CHECK(close(w->fd));
    Vec_free_f(Watcher, w->watchers, Watcher_free);
    return 0;
}

#endif // !DAEMON_H
