#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

typedef unsigned char byte;

typedef struct Gossip {
    int pipefd[2];
} Gossip;

int gossip_init(Gossip* g) {
    return pipe(g->pipefd);
}

int gossip_dup(Gossip* g, size_t i, int fd) {
    return dup2(g->pipefd[i], fd);
}

int gossip_close(Gossip* g, size_t i) {
    return close(g->pipefd[i]);
}

int gossip_write(Gossip* g, void* ptr, size_t sz) {
    ssize_t res, size = sz;
    byte* p = (byte*)ptr;
    do {
        res = write(g->pipefd[1], p, size);
        if(res < 0)
            return res; // fail
        p += res;
        size -= res;
    } while(size > 0);
    gossip_sync(g);
    return 0;
}

int gossip_read(Gossip* g, void* ptr, size_t sz) {
    ssize_t res, size = sz;
    byte* p = (byte*)ptr;
    do {
        res = read(g->pipefd[0], p, size);
        p += res;
        size -= res;
    } while(size > 0);
    gossip_sync(g);
    return 0;
}

int gossip_free(Gossip* g) {
    int res = gossip_close(g, 0);
    if(res != 0)
        return res;
    res = gossip_close(g, 1);
    return res;
}
