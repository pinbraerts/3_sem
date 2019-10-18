#include "../utils.h"

int main() {
    CHECK(mkfifo(NAME, 0666) == -1);

    printf("Server channel: %s\n", NAME);

    int fd;
    CHECK(fd = open(NAME, O_RDONLY));

    size_t n;
    ssize_t m;
    CHECK(m = read(fd, &n, sizeof(size_t)));

    printf("Total amount of bytes: %lu\n", n);

    char* base = (char*)malloc(n);
    if(base == NULL) {
        perror("malloc(n)");
        return 1;
    }
    char* input = base;

    while((m = read(fd, input, n)) > 0 && n > m) {
        printf("Server received %ld bytes: '", m);
        fwrite(input, 1, m, stdout);
        puts("'");
        input += m;
        n -= m;
    }

    free(base);

    CHECK(m);
    CHECK(close(fd));
    CHECK(remove(NAME));

    return 0;
}
