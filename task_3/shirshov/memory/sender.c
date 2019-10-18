#include <sys/shm.h>

#include "../utils.h"

int main(int argc, char** argv) {
    key_t key;
    CHECK(key = ftok("sender.c", 0));

    ssize_t n;
    FILE* f;
    f = fopen(argv[1], "r");
    if(f == NULL) {
        perror("fopen(argv[1], \"r\")");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    rewind(f);

    int shmid;
    CHECK(shmid = shmget(key, n, 0644 | IPC_CREAT));

    char* data = (char*)shmat(shmid, NULL, 0);
    fread(data, 1, n, f);

    sleep(5); // give receiver a time to launch (while reading memory shctl is blocked)

    CHECK(shmctl(shmid, IPC_RMID, NULL));

    return 0;
}
