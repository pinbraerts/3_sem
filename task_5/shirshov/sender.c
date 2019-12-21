#include "includes.h"

int main(int argc, char** argv) {
    pid_t p = atoi(argv[2]);

    int fd;
    CHECK(fd = open(argv[1], O_RDONLY));

    struct stat s;
    CHECK(fstat(fd, &s));

    char* buf = (char*)malloc(sizeof(char) * s.st_size);    CHECK(read(fd, buf, s.st_size));
 
    for(unsigned n = 0; n < s.st_size; ++n) {
        union sigval s;
        s.sival_int = n * 256 + buf[n];

        sigqueue(p, SIGUSR1, s);
        sleep(1);
    }
    kill(p, SIGTERM);

    return 0;
}
