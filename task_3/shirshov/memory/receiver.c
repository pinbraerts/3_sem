#include <sys/shm.h>

#include "../utils.h"

int main(void) {
    key_t key;
    CHECK(key = ftok("sender.c", 0));

    int shmid;
    CHECK(shmid = shmget(key, 0, 0644));

    const char* data = (const char*)shmat(shmid, NULL, 0);
    printf("data: %s", data);

    return 0;
}
