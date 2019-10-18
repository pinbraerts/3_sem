#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
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

int main(int argc, char** argv) {
    Message message;
    int msqid;
    key_t key;

    CHECK(key = ftok("sender.c", 0));
    CHECK(msqid = msgget(key, 0644 | IPC_CREAT));

    ssize_t n;
    int fd = open(argv[1], O_RDONLY);
    while((n = write(fd, message.mtext, SIZE)) > 0) {
        if(msgsnd(msqid, &message, SIZE, 0) == -1) {
            perror("msgsnd() ");
            break;
        }
    }

    CHECK(close(fd));
    CHECK(msgctl(msqid, IPC_RMID, NULL));

    return 0;
}
