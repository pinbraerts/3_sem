#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#define CHECK(...) do { if(( __VA_ARGS__ ) == -1) { perror(__FILE__ " " #__VA_ARGS__ " "); return -1; } } while(0)
#define NAME "/tmp/fifo.fifo"
#define SIZE 256

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
        printf("Server received: '");
        fwrite(message.mtext, 1, n, stdout);
        puts("'");
    }

    return 0;
}
