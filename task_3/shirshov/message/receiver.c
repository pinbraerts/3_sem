#include <sys/msg.h>

#include "../utils.h"

typedef struct {
    int mtype;
    char mtext[SIZE];
} Message;

int main() {
    Message message;
    int msqid;
    key_t key;
    CHECK(key = ftok("sender.c", 0));

    CHECK(msqid = msgget(key, 0644));

    ssize_t n;
    while((n = msgrcv(msqid, &message, SIZE, 0, 0)) > 0) {
        printf("Server received %ld bytes: '", n);
        fwrite(message.mtext, 1, n, stdout);
        puts("'");
    }

    return 0;
}
