#ifndef WATCHER_H
#define WATCHER_H

#include "includes.h"
#include "diff.h"

typedef struct {
    size_t start, end;
    String filename;
    String buffer;
} WatcherEdit;

void WatcherEdit_init(WatcherEdit* o) {
    o->start = 0;
    o->end = 0;
    String_init(o->filename);
    String_init(o->buffer);
}

int WatcherEdit_dump(WatcherEdit* o, int fd) {
    CHECK(write_all(fd, o, sizeof(size_t) * 2));
    CHECK(String_dump(o->filename, fd));
    CHECK(String_dump(o->buffer, fd));
    return 0;
}

int WatcherEdit_load(WatcherEdit* o, int fd) {
    CHECK(read_all(fd, 0, sizeof(size_t) * 2));
    CHECK(String_load(o->filename, fd));
    CHECK(String_load(o->buffer, fd));
}

void WatcherEdit_free(WatcherEdit* o) {
    o->start = 0;
    o->end = 0;
    String_free(o->filename);
    String_free(o->buffer);
}

typedef struct {
    String filename;
    String initial_buffer;
    String last_buffer;
    Vector edits; // Vector<Edit>
} Watcher;

void Watcher_init(Watcher* o, String* name) {
    String_init(o->filename);
    String_init(o->initial_buffer);
    String_init(o->last_buffer);
    pString_copy(&o->filename, name);
    Vector_init(o->edits);
}

int Watcher_rdbuf(Watcher* o, String* buf) {
    puts(o->filename.data);

    int fd;
    CHECK(fd = open(o->filename.data, O_RDONLY));

    ssize_t size;
    CHECK(size = lseek(fd, 0, SEEK_END));
    lseek(fd, 0, SEEK_SET);

    pString_resize(buf, size);
    CHECK(read_all(fd, buf->data, size));

    CHECK(close(fd));

    return 0;
}

void Watcher_start(Watcher* o) {
    Watcher_rdbuf(o, &o->initial_buffer);
    String_copy(o->last_buffer, o->initial_buffer);
}

void Watcher_free(Watcher* o) {
    String_free(o->filename);
    String_free(o->initial_buffer);
    String_free(o->last_buffer);
    Vector_free_f(Edit, o->edits, free);
}

void Watcher_diff(Watcher* o, Vector* edits) {
    String buf;
    String_init(buf);
    Watcher_rdbuf(o, &buf);
    puts(buf.data);

    Vector d = diff(o->last_buffer, buf);
    Vector_foreach(Edit, d, e) {
        WatcherEdit* edit = pVector_back_new(WatcherEdit, edits);
        WatcherEdit_init(edit);
        edit->start = e->x_start;
        edit->end = e->x_end;
        String_assign_n(edit->buffer, o->last_buffer.data + e->y_start, e->y_end - e->y_start);
        String_copy(edit->filename, o->filename);
    }
    Vector_free(d);
    String_free(buf);

    String_move(o->last_buffer, buf);
}

#endif // !WATCHER_H
