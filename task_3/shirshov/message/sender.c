#include <sys/msg.h>

#include "../utils.h"

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
    while((n = read(fd, message.mtext, SIZE)) > 0) {
        if(msgsnd(msqid, &message, SIZE, 0) == -1) {
            perror("msgsnd() ");
            break;
        }
    }

    CHECK(close(fd));
    CHECK(msgctl(msqid, IPC_RMID, NULL));

    return 0;
}
