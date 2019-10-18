#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define NAME "/tmp/fifo.fifo"
#define SIZE 256

int main() {
    char input[SIZE];
    if(mkfifo(NAME, 0666) == -1) {
        perror("mkfifo() ");
        return -1;
    }

    printf("Server channel: %s\n", NAME);
    int fd = open(NAME, O_RDONLY);

    if(fd == -1) {
        perror("open() ");
        return -2;
    }

    ssize_t n;
    while((n = read(fd, input, SIZE)) > 0) {
        printf("Server received: '");
        fwrite(input, 1, n, stdout);
        puts("'");
    }

    close(fd);
    remove(NAME);

    return 0;
}
